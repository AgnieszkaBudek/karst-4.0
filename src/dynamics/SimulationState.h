//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_SIMULATIONSTATE_H
#define KARST_4_0_SIMULATIONSTATE_H

#include "src/utils.h"
#include "src/units.h"

namespace karst {

    struct SimulationState {

        enum StateType {WARNING_STATE, NORMAL_STATE, BREAKTHROUGH_STATE};
        enum TimeAdaptationMode {T_ADAPT_INCREASE, T_ADAPT_DECREASE, T_ADAPT_NEUTRAL};

        // simulation state
        Long sim_step    {0};           ///< nr of simulation steps already done
        Time dt          {1};       	///< time step (in dimensionless units [2 k1 * gamma_1/d0])

        TimeAdaptationMode set_new_dt  {T_ADAPT_NEUTRAL};    ///< if change the time step
        StateType sim_state {NORMAL_STATE};                  ///< simulation state

    };

}  //namespace karst


#endif //KARST_4_0_SIMULATIONSTATE_H
