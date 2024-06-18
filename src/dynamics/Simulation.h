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
            const SimulationConfig        sym_conf;


            Configs(NetworkConfig&& net,
                    NetworkTopologyConfig&& net_topo,
                    PrintingConfig&& print,
                    SimulationConfig&& sym)
                    :
                    net_conf(std::move(net)),
                    net_topo_conf(std::move(net_topo)),
                    print_conf(std::move(print)),
                    sym_conf(std::move(sym)) {}
        };


    explicit Simulation(const std::string& cfile_name) : confs(read_configs(cfile_name)) {}

        const Configs confs;

    auto init() -> void {

    }

    protected:
        SimulationState s{};
        Network S = Network(confs.net_conf, confs.net_topo_conf, confs.print_conf );

    };

}  //namespace karst

#endif //KARST_4_0_SIMULATION_H
