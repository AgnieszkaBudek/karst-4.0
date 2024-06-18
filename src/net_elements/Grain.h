# ifndef KARST_4_0_GRAIN_H
# define KARST_4_0_GRAIN_H

#include "src/utils.h"
#include "src/units.h"
#include "src/net_elements/GenericElement.h"
#include "src/import_export/printing_primitives.h"

namespace karst {

    struct GrainState {

        std::map <SPECIES, Volume> v;      ///< Volume of specific species
        Volume max_volume {NaN};           ///< maximal volume of a grain
        Volume tot_volume {NaN};           ///< total volume of a grain

    };


    class Grain : public GenericElement <Grain, GrainState> {

        public:

            explicit Grain  (const Network &S0, const ElementConfig config0) : GenericElement<Grain, GrainState>(S0,config0){}


            friend  GenericElement <Grain, GrainState>;


            inline auto check_if_active() const    -> bool  {}       //TODO: implement it

            inline auto check_if_space_left() const -> bool   { return s.tot_volume < s.max_volume; }

            inline auto get_max_volume() const   -> Volume    { return s.max_volume; }

            inline auto get_tot_v() const        -> Volume    { return s.tot_volume; }

            inline auto get_v(SPECIES sp)  const -> Volume {
                assert(s.v.find(sp) != s.v.end() && "Key not found in the map");
                return s.v.at(sp);
            }

            inline auto set_v(SPECIES sp, Volume v_new) -> void { s.v[sp] = v_new; }



            inline auto calculate_tot_c () -> Volume
            {
                return std::accumulate(
                    s.v.begin(),
                    s.v.end(),
                    Volume(0.0),
                    [](Volume value, const std::pair<SPECIES, Volume>& p) {
                        return value + p.second;
                    });
            }

            auto init() -> void
            {
                s.max_volume = area(n);
                s.v = {{SPECIES::A, calculate_initial_vol()}};
                for (auto sp : solidS){
                    if (sp != SPECIES::A)
                        s.v[sp] = 0._V;
                }
            }


            // export functions

            friend log_stream& operator<<(log_stream& os, const Grain& obj) {
                os <<  obj.config.type << ": "<< obj.config.name << std::endl;
                os <<"\tVolume";
                for(auto&[key,value]: obj.s.v)
                    os << key <<" <-> "<<value<<"\t";
                os << "max_volume = "<<obj.s.max_volume <<" tot_volume"<<obj.s.tot_volume;
                os << std::endl<<std::endl;
                return os;
            }

            friend ofstream_ps_pores &operator <<(ofstream_ps_pores &stream, const Grain &g){}     //TODO: implement it
            friend ofstream_ps_grains &operator<<(ofstream_ps_grains &stream, const Grain &g){}    //TODO: implement it


    protected:


    };


} // namespace karst

# endif  //KARST_4_0_GRAIN_H