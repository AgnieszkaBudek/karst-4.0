//
// Created by Agnieszka on 19/06/2024.
//
#include <cxxopts.hpp>
#include "src/utils.h"
#include "src/simulation/Simulation.h"
#include "src/simulation/euler_step.h"
#include "src/network/network_topo_generator/create_hexagonal_network.h"
#include "src/network/network_topo_generator/read_from_csv.h"

using namespace karst;

int main(int argc, char** argv) {

    Logger<logger_level_min> log  {logger_output};
    // Default values
    bool show_picture = true;
    std::string config_name = "config.txt";

    try {
        cxxopts::Options options(argv[0], "Karst Simulation Program");

        options.add_options()
                ("P,show-picture", "Automatically show picture (true or false)", cxxopts::value<bool>(show_picture)->default_value("true"))
                ("C,config", "Path to the config file", cxxopts::value<std::string>(config_name))
                ("h,help", "Print help");

        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        // The values have been automatically assigned to the variables:
        // show_picture, config_path, sim_version

        // Check if the mandatory config file path was provided
        if (config_name.empty()) {
            std::cerr << "Error: Path to the config file is required. Use the -C or --config option." << std::endl;
            std::cerr << "Use --help to see available options." << std::endl;
            return 1;
        }


    } catch (const cxxopts::exceptions::exception& e) {
        std::cerr << "Error parsing options: " << e.what() << std::endl;
        return 1;
    }


    Simulation simulation{config_name,log};
    simulation.init();
    simulation.run_simulation();


    log.log("Preparing pictures...");
    system("ps2pdf *.ps");




    return 0;
}
