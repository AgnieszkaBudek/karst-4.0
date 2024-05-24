# ifndef KARST_4_0_GRAIN_H
# define KARST_4_0_GRAIN_H

#include "src/utils.h"
#include "src/units.h"
#include "src/net_elements/GenericElement.h"

namespace karst {

    struct GrainState {

        std::map <SolidChemical, Volume> v;               ///< Volume of specific species
        Volume max_volume {NaN};                          ///< maximal volume of a grain
        Volume tot_v {NaN};                               ///< total volume of a grain

    };


    class Grain : public GenericElement <Grain, GrainState> {



        public:

            friend  GenericElement <Grain, GrainState>;

            auto check_if_active() -> bool
            {

            }

            auto check_if_space_left() -> bool
            {
                return s.tot_v > Volume {0};
            }

            auto get_max_volume() -> Volume
            {
                return s.max_volume;
            }

        protected:

    };


} // namespace karst

# endif  //KARST_4_0_GRAIN_H