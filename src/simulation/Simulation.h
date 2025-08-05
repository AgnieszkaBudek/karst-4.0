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



    explicit Simulation(const std::string& cfile_name) : confs(tupleToStruct<Simulation::Configs>(read_configs(cfile_name))) {
//        switch (confs.sim_conf.integration_mode) { //TODO: figure out how to choose the integration method...
//            case (INTEGRATION_METHOD::EULER) :
//
//        }
    }


    const Configs confs;


    auto init()             -> void { do_init();}
    auto run_simulation()   -> void {do_run_simulation();}
    auto get_state() const  -> const SimulationState& {return s;}

    protected:

        SimulationState s {};
        Network N {confs.net_conf, confs.net_topo_conf, confs.print_conf };
        ReactionKinetics R  {N, confs.sim_conf, s};


        auto check_simulation_state()   -> void {}  //TODO: implement later
        auto check_dt_adaptation_mode() -> void {}  //TODO: implement later

        auto save_results()             -> void {}  //TODO: implement later


        auto do_init() -> void {

            std::cerr<<"Initializing simulation..."<<std::endl;
            N.init();
            s.dt = confs.sim_conf.dt0;
            N.save_network_state();

            if(confs.net_conf.reaction_set == ReactionSet::LINEAR_DP)
            R.prepare_linear_kinetics();
            else {std::cerr<<"ERROR: Reactions not implemented...";}

            if(!R.check_implementation()) std::cerr<<"ERROR: Problem with reactions implementation...";
        }

        auto do_run_simulation() -> void{

            while(s.sim_state==SimulationStateType::NORMAL){

                s.T = s.T + s.dt;
                s.sim_step++;

                save_results();
                check_simulation_state();
                check_dt_adaptation_mode();
            }
        }

    };

}  //namespace karst

#endif //KARST_4_0_SIMULATION_H
