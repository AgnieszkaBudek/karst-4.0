//
// Created by Agnieszka on 23/05/2024.
//

#ifndef KARST_4_0_NETWORK_STATE_H
#define KARST_4_0_NETWORK_STATE_H

#include "src/utils.h"
#include "src/units.h"

namespace karst {


    struct NetworkState {



        // flow parameter
        Pressure P_in  {10};     ///< pressure at the inlet (must by positive)
        Pressure P_out {0};	     ///< pressure at the outlet, always should be set to zero
        Flow     Q_tot {10};     ///< total flow through the system (if == 0 the constant pressure is kept)


        // chemical properties
        std::map <SPECIES, Concentration>   C_0 ;	      ///< inlet concentration of all soluble chemicals
        std::map <SPECIES, Volume>          V_tot {};      ///< total volume of all species

    };

}

#endif //KARST_4_0_NETWORK_STATE_H
