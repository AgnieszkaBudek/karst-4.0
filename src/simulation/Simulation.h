//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_SIMULATION_H
#define KARST_4_0_SIMULATION_H

#include "src/utils.h"
#include "src/units.h"
#include "src/network/Network.h"
#include "src/simulation/SimulationConfig.h"
#include "src/simulation/SimulationState.h"
#include "src/read_config.h"
#include "src/chemistry/prepare_chemical_kinetics.h"

#include "src/network/network_topo_generator/network_topo_generator.h"

#include "src/simulation/SimulationSteps/SimulatePressure.h"
#include "src/simulation/SimulationSteps/SimulateFlow.h"
#include "src/simulation/SimulationSteps/SimulateConcentration.h"
#include "src/simulation/SimulationSteps/SimulateGeometry.h"
#include "src/simulation/SimulationSteps/SimulateMerging.h"


namespace karst {



    class Simulation {

    public:

        struct Configs{
            const NetworkConfig           net_conf;
            const NetworkTopologyConfig   net_topo_conf;
            const PrintingConfig          print_conf;
            const SimulationConfig        sim_conf;

            Configs(NetworkConfig&& net,
                    NetworkTopologyConfig&& net_topo,
                    PrintingConfig&& print,
                    SimulationConfig&& sim)
                    :
                    net_conf(std::move(net)),
                    net_topo_conf(std::move(net_topo)),
                    print_conf(std::move(print)),
                    sim_conf(std::move(sim))
                    {}
        };


    explicit Simulation(const std::string& cfile_name) : confs(tupleToStruct<Simulation::Configs>(read_configs(cfile_name))) {}



    const Configs confs;
    const SimulationConfig&      sim_conf       = confs.sim_conf;
    const NetworkConfig&         net_conf       = confs.net_conf;
    const PrintingConfig&        print_conf     = confs.print_conf;
    const NetworkTopologyConfig& net_topo_conf  = confs.net_topo_conf;

    auto init()             -> void { do_init();}
    auto run_simulation()   -> void { do_run_simulation();}
    auto get_state() const  -> const SimulationState& {return state;}

    protected:

        SimulationState state {};
        Network S {net_conf, net_topo_conf, print_conf };
        ReactionKinetics R  {S, confs.sim_conf, state};

        SimulatePressure        sim_pressure        {SimulatePressure       ::Config{.S=S,.R=R,.sim_config=sim_conf, .sim_state=state}};
        SimulateFlow            sim_flow            {SimulateFlow           ::Config{.S=S,.R=R,.sim_config=sim_conf, .sim_state=state}};
        SimulateConcentrations  sim_concentration   {SimulateConcentrations ::Config{.S=S,.R=R,.sim_config=sim_conf, .sim_state=state}};
        SimulateGeometry        sim_geometry        {SimulateGeometry       ::Config{.S=S,.R=R,.sim_config=sim_conf, .sim_state=state}};



        auto do_init() -> void {

            // 1. Initializing Network
            std::cerr<<"Initializing simulation..."<<std::endl;
            S.init();
            state.dt = sim_conf.dt0;
            S.save_network_state();

            // 2. Preparing reactions
            switch(confs.net_conf.reaction_set){
                case ReactionSet::LINEAR_DP:  R.prepare_linear_dissolution_and_precipitation(); break;
                case ReactionSet::LINEAR_D:   R.prepare_linear_dissolution(); break;
                case ReactionSet::SIZE:       break;
            }
            if(!R.check_implementation()) std::cerr<<"ERROR: Problem with reactions implementation...";

            // 3. Preparing Simulation steps
            sim_pressure     .init();
            sim_flow         .init();
            sim_concentration.init();
            sim_geometry     .init();

        }

        auto do_run_simulation() -> void{


            while(state.sim_state==SimulationStateType::NORMAL){

                state.T = state.T + state.dt;
                state.sim_step++;
                std::cerr<<"\n\n"<<state.sim_step<<". step of Simulation, ";
                std::cerr<<"T = "<<state.T<<"."<<std::endl;

                do_one_Euler_step();

                save_results();
                check_simulation_state();
                adapt_time_step();
            }
        }


        auto do_one_Euler_step() -> void;


        auto check_simulation_state()   -> void {  //TODO: implement later

            if(state.T >= confs.sim_conf.T_max) {
                state.sim_state = SimulationStateType::FINISHED;
                std::cerr << "Setting simulation state to " << state.sim_state << std::endl;
            }
        }

        auto adapt_time_step() -> void {}  //TODO: implement later

        auto save_results()             -> void {}  //TODO: implement later



    };

}  //namespace karst

#endif //KARST_4_0_SIMULATION_H
