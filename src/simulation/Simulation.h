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


    explicit Simulation(const std::string& cfile_name, Logger<logger_level_min>& log0):
        confs(tupleToStruct<Simulation::Configs>(read_configs(cfile_name,log0))), log{log0} {}



    const Configs confs;
    const SimulationConfig&      sim_conf       = confs.sim_conf;
    const NetworkConfig&         net_conf       = confs.net_conf;
    const PrintingConfig&        print_conf     = confs.print_conf;
    const NetworkTopologyConfig& net_topo_conf  = confs.net_topo_conf;

    auto init()             -> void { do_init();}
    auto run_simulation()   -> void { do_run_simulation();}
    auto get_state() const  -> const SimulationState& {return state;}

    protected:

        Logger<logger_level_min>&  log;

        PrintingModule   io    {confs.net_topo_conf, confs.print_conf, log};
        SimulationState  state {};
        Network          S     {net_conf, net_topo_conf, print_conf, log, io};
        ReactionKinetics R     {S, confs.sim_conf, state};

        SimulatePressure        sim_pressure        {SimulatePressure       ::Config{.S=S, .R=R, .sim_config=sim_conf, .sim_state=state, .log = log}};
        SimulateFlow            sim_flow            {SimulateFlow           ::Config{.S=S, .R=R, .sim_config=sim_conf, .sim_state=state, .log = log}};
        SimulateConcentrations  sim_concentration   {SimulateConcentrations ::Config{.S=S, .R=R, .sim_config=sim_conf, .sim_state=state, .log = log}};
        SimulateGeometry        sim_geometry        {SimulateGeometry       ::Config{.S=S, .R=R, .sim_config=sim_conf, .sim_state=state, .log = log}};


        auto do_init() -> void {

            // 1. Initializing Network
            log.log<LogLevel::INFO>("Initializing simulation...");
            S.init();
            state.dt = sim_conf.dt0;
            S.save_network_state("After Network.init().");

            // 2. Preparing reactions
            switch(confs.net_conf.reaction_set){
                case ReactionSet::LINEAR_DP:  R.prepare_linear_dissolution_and_precipitation(); break;
                case ReactionSet::LINEAR_D:   R.prepare_linear_dissolution(); break;
                case ReactionSet::SIZE:       break;
            }
            if(!R.check_implementation()) S.log.log<LogLevel::ERROR>("Problem with reactions implementation...");

            // 3. Preparing Simulation steps
            sim_pressure     .init();
            sim_flow         .init();
            sim_concentration.init();
            sim_geometry     .init();

        }

        auto do_run_simulation() -> void{

            if(logger_level_min<= LogLevel::DEBUG) sim_geometry.ps_for_debug();

            while(state.sim_state==SimulationStateType::NORMAL){

                log.pure_log<LogLevel::INFO>("\n\n"+std::string(50, '*')+"\n");
                log.log<LogLevel::INFO>(std::to_string(state.sim_step)+". step of Simulation.");
                log.log<LogLevel::INFO>("T = "+state.T+"\n");

                do_one_Euler_step();

                save_results();
                check_simulation_state();
                adapt_time_step();


            }
        }


        auto do_one_Euler_step() -> void;
        auto do_one_LeapFrog_step() -> void;
        auto do_iterative_step() -> void;       //to be implemented


        auto check_simulation_state()   -> void {  //TODO: implement later

            if(state.T >= confs.sim_conf.T_max) {
                state.sim_state = SimulationStateType::FINISHED;
                log.log<LogLevel::INFO>( "Setting simulation state to " + state.sim_state);
            }
        }

        auto adapt_time_step() -> void {}  //TODO: implement later

        auto save_results()             -> void {}  //TODO: implement later


    };

}  //namespace karst

#endif //KARST_4_0_SIMULATION_H
