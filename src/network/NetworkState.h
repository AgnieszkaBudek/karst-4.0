//
// Created by Agnieszka on 23/05/2024.
//

#ifndef KARST_4_0_NETWORK_STATE_H
#define KARST_4_0_NETWORK_STATE_H

#include "src/utils.h"
#include "src/units.h"

namespace karst {


    struct NetworkState {

        enum TimeAdaptationMode {T_ADAPT_INCREASE, T_ADAPT_DECREASE, T_ADAPT_NEUTRAL};

        // simulation state
        Long sim_step    {0};           ///< nr of simulation steps already done
        Time dt          {1};       	///< time step (in dimensionless units [2 k1 * gamma_1/d0])
        TimeAdaptationMode set_new_dt  {T_ADAPT_NEUTRAL};    ///< if change the time step

        // flow parameter

        Pressure P_in {10};     ///< pressure at the inlet (must by positive)
        Pressure P_out {0};	    ///< pressure at the outlet, always should be set to zero
        Flow     Q_tot {10};    ///< total flow through the system (if == 0 the constant pressure is kept)


        // properties
        std::map <SolidChemical, Volume> V_tot {};      ///< total volume of all species

    };

}

#endif //KARST_4_0_NETWORK_STATE_H
