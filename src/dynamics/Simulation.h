//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_SIMULATION_H
#define KARST_4_0_SIMULATION_H

#include "src/utils.h"
#include "src/units.h"
#include "src/network/Network.h"
#include "src/dynamics/SimulationConfig.h"
#include "src/dynamics/SimulationState.h"
#include "src/read_config.h"
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


    explicit Simulation(const std::string& cfile_name) : confs(read_configs(cfile_name)) {
//        switch (confs.sim_conf.integration_mode) { //TODO: figure out how to choose the integration method...
//            case (INTEGRATION_METHOD::EULER) :
//
//        }
    }

        const Configs confs;

    auto init() -> void {
        S.init();
        s.dt = confs.sim_conf.dt0;

        S.save_network_state();
    }

    auto run_simulation() -> void {
        do_run_simulation();
    }

    protected:
        SimulationState s{};
        Network S = Network( confs.net_conf, confs.net_topo_conf, confs.print_conf );



        auto check_simulation_state()   -> void {}  //TODO: implement later
        auto check_dt_adaptation_mode() -> void {}  //TODO: implement later

        auto save_results()             -> void {}  //TODO: implement later


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
