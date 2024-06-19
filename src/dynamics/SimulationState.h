//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_SIMULATIONSTATE_H
#define KARST_4_0_SIMULATIONSTATE_H

#include "src/utils.h"
#include "src/units.h"

namespace karst {

    enum class SimulationStateType          {WARNING, NORMAL, BREAKTHROUGH};
    // Specialization of EnumToString
    template<>
    const std::map<SimulationStateType, std::string> EnumToString<SimulationStateType>::mapping = {
            {SimulationStateType::WARNING,      "WARNING" },
            {SimulationStateType::NORMAL,       "NORMAL" },
            {SimulationStateType::BREAKTHROUGH, "BREAKTHROUGH" }
    };
//    // Operator << specialization for SimulationStateType     //TODO: wyrzucić potem
//    std::ostream& operator<<(std::ostream& os, SimulationStateType value) {
//        return os << EnumToString<SimulationStateType>::mapping.at(value);
//    }


    enum class TimeAdaptationMode {INCREASE, DECREASE, NEUTRAL};
    template<>
    const std::map<TimeAdaptationMode, std::string> EnumToString<TimeAdaptationMode>::mapping = {
            {TimeAdaptationMode::INCREASE,  "INCREASE" },
            {TimeAdaptationMode::DECREASE,  "DECREASE" },
            {TimeAdaptationMode::NEUTRAL,   "NEUTRAL" }
    };
//    // Operator << specialization for SimulationStateType     //TODO: wyrzucić potem
//    std::ostream& operator<<(std::ostream& os, TimeAdaptationMode value) {
//        return os << EnumToString<TimeAdaptationMode>::mapping.at(value);
//    }



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
