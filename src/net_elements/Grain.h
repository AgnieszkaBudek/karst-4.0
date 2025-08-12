# ifndef KARST_4_0_GRAIN_H
# define KARST_4_0_GRAIN_H

#include "src/utils.h"
#include "src/units.h"
#include "src/net_elements/GenericElement.h"
//#include "src/import_export/printing_primitives.h"

namespace karst {

    struct GrainState {

        EnumArray <SOLIDS, Volume, enum_size_v<SOLIDS>> v{0._V};   ///< Volume of specific species
        Volume max_volume {NaN};                                   ///< maximal volume of a grain (corresponding to zero porosity)
        Volume tot_volume {NaN};                                   ///< total volume of a grain

    };


    class Grain : public GenericElement <Grain, GrainState> {

        friend  GenericElement <Grain, GrainState>;

        public:

            explicit Grain  (const NetworkConfig & net_conf0, const NetworkTopologyConfig &topo_conf0, const ElementConfig config0) : GenericElement<Grain, GrainState>(net_conf0,topo_conf0,config0){}


            auto check_if_space_left () const -> bool      {
                return state.tot_volume < state.max_volume; }

            auto get_max_volume      () const -> Volume    {
                ASSERT_MSG(state.max_volume>=0._V, get_context_info());
                return state.max_volume; }

            auto get_tot_v           () const -> Volume    {
                ASSERT_MSG(state.tot_volume>=0._V, get_context_info());
                return state.tot_volume; }

            auto if_species_available     (SOLIDS sp) const -> bool   { return state.v[sp] > 0._V; }        //can take into account passivation or nucleation

            auto get_v(SOLIDS sp)  const -> Volume {
                ASSERT_MSG(state.v[sp]>=0._V, get_context_info());
                return state.v[sp];
            }

            auto set_v(SOLIDS sp, Volume v_new) -> void { state.v[sp] = v_new; }
            auto add_v(SOLIDS sp, Volume v_new) -> void { state.v[sp] +=  v_new; }
            auto set_tot_v(Volume v_new) -> void { state.tot_volume =  v_new; }

            auto calculate_tot_v () -> Volume{
                Volume V_tot {0.};
                for(auto v : state.v.get_data())
                    V_tot+=v;
                return V_tot;


            }

            //friend ofstream_ps_pores &operator <<(ofstream_ps_pores &stream, const Grain &g){}     //TODO: implement it
            //friend ofstream_ps_grains &operator<<(ofstream_ps_grains &stream, const Grain &g){}    //TODO: implement it


    protected:

        void do_disconnect_from_network();

        auto do_init() -> void
        {
            state.max_volume = calculate_maximal_volume();

            Volume tmp = calculate_initial_vol();
            state.v[SOLIDS::X] = tmp * net_config.X_f;
            state.v[SOLIDS::A] = tmp - state.v[SOLIDS::X];
            for (auto sp : net_config.solidS){
                if (sp != SOLIDS::A and sp!= SOLIDS::X)
                    state.v[sp]=0.0_V;
            }
            state.tot_volume = tmp;
        }


        auto calculate_maximal_volume() -> Volume{      //FIXME: take into account periodic boundary conditions!!!
            Area area {0.};
            int n = int(nodes.size());
            for (auto i = 0; i < n; ++i) {
                int j = (i + 1) % n;
                area +=  nodes[i]->get_pos().x * nodes[j]->get_pos().y;  // suma x_i * y_{i+1}
                area += -nodes[j]->get_pos().x * nodes[i]->get_pos().y;  // suma x_i * y_{i+1}
            }
            return net_config.h_tot*abs(area) / 2.0;
        }


        auto calculate_initial_vol() -> Volume{
            auto vals = pores
                        | std::views::filter([](auto& p){ return p->get_type() == PoreType::MATRIX; })
                        | std::views::transform([](auto& p){ return p->get_d(); });


            Length d_mean = std::ranges::empty(vals) ? 0._L :
                            std::ranges::fold_left(vals, 0._L, [](Length a, Length b)
                            {return a+b;}) / double(std::ranges::distance(vals));
            return  power<std::ratio<3,2>>(4._U- (net_config.h_tot*power<2>(d_mean)*M_PI)/state.max_volume)
                    * state.max_volume / 8.;
        }

        auto do_is_state_set() -> bool{

            return (
                    !std::isnan(double(state.max_volume)) and
                    !std::isnan(double(state.tot_volume)) and
                    std::ranges::all_of(state.v.get_data(), [](const auto& x) {
                        return double(x) >= 0;
                    })
                    );
        }

        std::string do_get_state_info() const{
            std:: string str =  "\n.max_volume = " + state.max_volume +
                                "\n.tot_volume = " + state.tot_volume;
            for (auto sp : net_config.solidS)
                str  += "\tc[" + sp + "]        = "+state.v[sp];
            return str;
        }

    };





} // namespace karst

# endif  //KARST_4_0_GRAIN_H