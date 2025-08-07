//
// Created by Agnieszka on 24/05/2024.
//
#include"src/simulation/Simulation.h"

#ifndef KARST_4_0_EULER_STEP_H
#define KARST_4_0_EULER_STEP_H

namespace karst {

    auto Simulation::do_one_Euler_step() -> void{

        sim_pressure        .run(state.sim_step);
        sim_flow            .run(state.sim_step);
        sim_concentration   .run(state.sim_step);
        sim_geometry        .run(state.sim_step);

    }

}
#endif //KARST_4_0_EULER_STEP_H
