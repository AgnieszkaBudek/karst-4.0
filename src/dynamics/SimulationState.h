//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_SIMULATIONSTATE_H
#define KARST_4_0_SIMULATIONSTATE_H

#include "src/utils.h"
#include "src/units.h"

namespace karst {



    struct SimulationState {


        // simulation state
        Long sim_step    {0};           ///< nr of simulation steps already done
        Time T           {0};           ///< Total time from the begging of simulation
        Time dt          {NaN} ;       	///< time step (in dimensionless units [2 k1 * gamma_1/d0])

        TimeAdaptationMode   set_new_dt  {TimeAdaptationMode::NEUTRAL};    ///< if change the time step
        SimulationStateType  sim_state   {SimulationStateType::NORMAL};    ///< simulation state

    };

}  //namespace karst


#endif //KARST_4_0_SIMULATIONSTATE_H
