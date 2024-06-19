//
// Created by Agnieszka on 19/06/2024.
//


#include "utils.h"
#include "src/dynamics/Simulation.h"

namespace karst {

    int main() {

        std::string config_name = "config.txt";
        Simulation simulation{config_name};
        simulation.init();


        return 0;
    }

}