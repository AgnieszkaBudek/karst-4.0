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
                //ASSERT_MSG(state.v[sp]>=0._V, get_context_info());
                return state.v[sp];
            }

            auto set_v(SOLIDS sp, Volume v_new) -> void { state.v[sp] =   v_new; }
            auto add_v(SOLIDS sp, Volume v_new) -> void { state.v[sp] +=  v_new; }
            auto set_tot_v(Volume v_new) -> void { state.tot_volume =     v_new; }
            auto set_v_max(Volume v_new) -> void { state.max_volume =     v_new; }
            auto update_v_tot() -> void { state.tot_volume = calculate_tot_v();}

            auto calculate_tot_v () -> Volume{
                Volume V_tot {0.};
                for(auto v : state.v.get_data())
                    V_tot+=v;
                ASSERT_MSG(V_tot>=0._V, get_context_info());
                return V_tot;
            }

            bool is_crossing_pbc() const{
                //don't draw polygons crossing periodic boundary
                auto max_l = 0._L;          //maximal distance between consecutive nodes;
                for(auto it = nodes.begin(); it<nodes.end()-1; ++it)
                    if((*it)->get_pos() -(*(it+1))->get_pos() > max_l )
                        max_l = (*it)->get_pos() -(*(it+1))->get_pos();
                if(max_l > Unitless (topo_config.N_x) * 0.5_U * net_config.l0)
                    return true;
                else
                    return false;
            }

            //friend ofstream_ps_pores &operator <<(ofstream_ps_pores &stream, const Grain &g){}     //TODO: implement it
            //friend ofstream_ps_grains &operator<<(ofstream_ps_grains &stream, const Grain &g){}    //TODO: implement it


    protected:

        void do_disconnect_from_network();

        auto do_init() -> void
        {
            if(is_state_set())  return;        //do nth is initial state is already set

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


        auto calculate_maximal_volume() -> Volume{      //only for 2D simulations, periodic boundary conditions taken into account

            //if(get_nodes().size()<=2)   return state.max_volume;
            if(topo_config.type_of_topology==TypeOfNetTopology::CUBIC) return 1._V; //todo: Implement the proper volume calculation for cubic network

            Area area {0.};
            int n = int(nodes.size());

            std::vector<Length> x (n);
            std::vector<Length> y (n);

            Length net_size_x = topo_config.N_x*net_config.l0;
            Length net_size_y = topo_config.N_y*net_config.l0;

            for (auto i = 0; i < n; ++i) {
                x[i] = nodes[i]->get_pos().x;
                y[i] = nodes[i]->get_pos().y;
            }

            if(topo_config.do_periodic_bc) {        //shift for periodic boundary condition
                if (std::ranges::max(x) - std::ranges::min(x) > net_size_x / 2.) {
                    for (auto &xx: x)
                        if (xx > net_size_x / 2.) xx += -net_size_x;
                }
                if (std::ranges::max(y) - std::ranges::min(y) > net_size_y / 2.) {
                    for (auto &yy: y)
                        if (yy > net_size_y / 2.) yy += -net_size_y;
                }
            }

            for (auto i = 0; i < n; ++i) {
                int j = (i + 1) % n;
                area += x[i]*y[j]-x[j]*y[i];  // suma x_i * y_{i+1}
            }
            config.log.log_state(*this,std::format("V_tot = {}",double(net_config.h_tot*abs(area) / 2.0)));
            return net_config.h_tot*abs(area) / 2.0;
        }


        auto calculate_initial_vol() -> Volume{

            if(topo_config.type_of_topology==TypeOfNetTopology::CUBIC) return 1._V; //todo: Implement the proper volume calculation for cubic network

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
                str  += "\nv[" + sp + "]        = "+state.v[sp];
            return str;
        }




    };





} // namespace karst

# endif  //KARST_4_0_GRAIN_H