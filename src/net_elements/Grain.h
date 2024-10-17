# ifndef KARST_4_0_GRAIN_H
# define KARST_4_0_GRAIN_H

#include "src/utils.h"
#include "src/units.h"
#include "src/net_elements/GenericElement.h"
//#include "src/import_export/printing_primitives.h"

namespace karst {

    struct GrainState {

        std::map <SPECIES, Volume> v;      ///< Volume of specific species
        Volume max_volume {NaN};           ///< maximal volume of a grain
        Volume tot_volume {NaN};           ///< total volume of a grain

    };


    class Grain : public GenericElement <Grain, GrainState> {

        public:

            explicit Grain  (const NetworkConfig & net_conf0, const NetworkTopologyConfig &topo_conf0, const ElementConfig config0) : GenericElement<Grain, GrainState>(net_conf0,topo_conf0,config0){}


            friend  GenericElement <Grain, GrainState>;


            inline auto check_if_active() const    -> bool  { return true;}       //TODO: implement it

            inline auto check_if_space_left() const -> bool   { return state.tot_volume < state.max_volume; }

            inline auto get_max_volume() const   -> Volume    { return state.max_volume; }

            inline auto get_tot_v() const        -> Volume    { return state.tot_volume; }

            inline auto get_v(SPECIES sp)  const -> Volume {
                assert(state.v.find(sp) != state.v.end() && "Key not found in the map");
                return state.v.at(sp);
            }

            inline auto set_v(SPECIES sp, Volume v_new) -> void { state.v[sp] = v_new; }



            inline auto calculate_tot_v () -> Volume
            {
                return std::accumulate(
                        state.v.begin(),
                        state.v.end(),
                        Volume(0.0),
                        [](Volume value, const std::pair<SPECIES, Volume>& p) {
                        return value + p.second;
                    });
            }

            auto init() -> void
            {
                //s.max_volume = area(n);
                //s.v = {{SPECIES::A, calculate_initial_vol()}};
                for (auto sp : solidS){
                    if (sp != SPECIES::A)
                        state.v[sp] = 0.0_V;
                }
            }


            // export functions

            friend log_stream& operator<<(log_stream& os, const Grain& obj) {
                os <<  obj.config.type << ": "<< obj.config.name << std::endl;
                os <<"\tVolume";
                for(auto&[key,value]: obj.state.v)
                    os << key <<" <-> "<<value<<"\t";
                os << "max_volume = " << obj.state.max_volume << " tot_volume" << obj.state.tot_volume;
                os << std::endl<<std::endl;
                return os;
            }

            //friend ofstream_ps_pores &operator <<(ofstream_ps_pores &stream, const Grain &g){}     //TODO: implement it
            //friend ofstream_ps_grains &operator<<(ofstream_ps_grains &stream, const Grain &g){}    //TODO: implement it


    protected:


    };


} // namespace karst

# endif  //KARST_4_0_GRAIN_H