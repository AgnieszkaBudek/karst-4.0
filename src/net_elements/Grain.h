# ifndef KARST_4_0_GRAIN_H
# define KARST_4_0_GRAIN_H

#include "src/utils.h"
#include "src/units.h"
#include "src/net_elements/GenericElement.h"
//#include "src/import_export/printing_primitives.h"

namespace karst {

    struct GrainState {

        EnumArray <SOLIDS, Volume, enum_size_v<SOLIDS>> v{0._V};      ///< Volume of specific species
        Volume max_volume {NaN};           ///< maximal volume of a grain (corresponding to zero porosity)
        Volume tot_volume {NaN};           ///< total volume of a grain

    };


    class Grain : public GenericElement <Grain, GrainState> {

        friend  GenericElement <Grain, GrainState>;

        public:

            explicit Grain  (const NetworkConfig & net_conf0, const NetworkTopologyConfig &topo_conf0, const ElementConfig config0) : GenericElement<Grain, GrainState>(net_conf0,topo_conf0,config0){}


            auto check_if_space_left () const -> bool      { return state.tot_volume < state.max_volume; }
            auto get_max_volume      () const -> Volume    { return state.max_volume; }
            auto get_tot_v           () const -> Volume    { return state.tot_volume; }
            auto if_species_left     (SOLIDS sp) const -> bool   { return state.v[sp] > 0._V; }

            auto get_v(SOLIDS sp)  const -> Volume {
                return state.v[sp];
            }

            auto set_v(SOLIDS sp, Volume v_new) -> void { state.v[sp] = v_new; }
            auto add_v(SOLIDS sp, Volume v_new) -> void { state.v[sp] +=  v_new; }


            auto calculate_tot_v () -> Volume
            {
                Volume V_tot {0.};
                for(auto v : state.v.get_data())
                    V_tot+=v;
                return V_tot;


            }


            // export functions

            friend log_stream& operator<<(log_stream& os, const Grain& obj) {
                os <<  obj.config.type << ": "<< obj.config.name << std::endl;
                os <<"\tVolume";
                for(auto[key,value]: obj.state.v.entries())
                    os << key <<" <-> "<<value<<"\t";
                os << "max_volume = " << obj.state.max_volume << " tot_volume" << obj.state.tot_volume;
                os << std::endl<<std::endl;
                return os;
            }

            //friend ofstream_ps_pores &operator <<(ofstream_ps_pores &stream, const Grain &g){}     //TODO: implement it
            //friend ofstream_ps_grains &operator<<(ofstream_ps_grains &stream, const Grain &g){}    //TODO: implement it


    protected:

        void do_disconnect_from_network();

        auto do_init() -> void
        {
            //state.max_volume = area(nodes);
            //state.v = {{SPECIES::A, calculate_initial_vol()}};
            for (auto sp : net_config.solidS){
                if (sp != SOLIDS::A)
                    state.v[sp]=0.0_V;
            }
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

    };


} // namespace karst

# endif  //KARST_4_0_GRAIN_H