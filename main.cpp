//
// Created by Agnieszka on 19/06/2024.
//


#include "src/utils.h"
#include "src/dynamics/Simulation.h"
#include "src/network/network_topo_generator/create_hexagonal_network.h"

using namespace karst;

int main() {

    std::string config_name = "config.txt";
    Simulation simulation{config_name};
    simulation.init();
    //simulation.run_simulation();

    std::cerr<<"Preparing pictures..."<<std::endl;
    system("ps2pdf net_pores.ps");
    system("open net_pores.pdf");


    return 0;
}

