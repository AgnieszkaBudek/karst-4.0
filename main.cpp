//
// Created by Agnieszka on 19/06/2024.
//


#include "src/utils.h"
#include "src/simulation/Simulation.h"
#include "src/simulation/euler_step.h"
#include "src/network/network_topo_generator/create_hexagonal_network.h"

using namespace karst;

int main() {

    Logger<logger_level_min> log  {logger_output};

    std::string config_name = "config.txt";
    Simulation simulation{config_name,log};
    simulation.init();
    simulation.run_simulation();

    log.log<LogLevel::INFO>("Preparing pictures...");
    system("ps2pdf *.ps");




    return 0;
}

