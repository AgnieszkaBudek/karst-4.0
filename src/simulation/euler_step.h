//
// Created by Agnieszka on 24/05/2024.
//
#include"src/simulation/Simulation.h"

#ifndef KARST_4_0_EULER_STEP_H
#define KARST_4_0_EULER_STEP_H

namespace karst {

    auto Simulation::do_one_Euler_step() -> void{

        sim_pressure        .run(state.sim_step,state.T);
        sim_flow            .run(state.sim_step,state.T);
        sim_concentration   .run(state.sim_step,state.T);

        state.T += state.dt;
        state.sim_step++;

        sim_geometry        .run(state.sim_step,state.T);
        //sim_merging         .run(state.sim_step,state.T); //to be implemented later...

    }

    auto Simulation::do_one_LeapFrog_step() -> void{ //I should definitely rethink this algorithm...

        sim_pressure        .run(state.sim_step,state.T);   sim_pressure.     update_old_state();
        sim_flow            .run(state.sim_step,state.T);   sim_flow.         update_old_state();
        sim_concentration   .run(state.sim_step,state.T);   sim_concentration.update_old_state();

        state.T += state.dt;
        state.sim_step ++;

        sim_geometry        .run(state.sim_step,state.T);

        sim_pressure        .run(state.sim_step,state.T);   sim_pressure.     mix_states();
        sim_flow            .run(state.sim_step,state.T);   sim_flow.         mix_states();
        sim_concentration   .run(state.sim_step,state.T);   sim_concentration.mix_states();

        sim_geometry        .go_back();         //going back with l and d
        sim_geometry        .run(state.sim_step,state.T);   sim_geometry.update_old_state();
        //sim_merging       .run(state.sim_step,state.T);       //to be implemented later...

    }

    auto Simulation::do_iterative_step() -> void{
        log.log<LogLevel::ERROR>("Simulation::do_iterative_step: Not yet implemented.");
    }

}
#endif //KARST_4_0_EULER_STEP_H
