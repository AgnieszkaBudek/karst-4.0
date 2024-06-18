//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_READ_CONFIG_H
#define KARST_4_0_READ_CONFIG_H

#include "src/utils.h"
#include "src/units.h"
#include "src/dynamics/Simulation.h"

namespace karst {

     auto read_configs(const std::string& f_path) -> Simulation::Configs {


        // Creating all configs
        NetworkConfig net_conf {};
        NetworkTopologyConfig net_top_conf {};
        PrintingConfig print_conf {};
        SimulationConfig sim_conf {};

        // reading config file:

         std::ifstream fp_setup(f_path);
         if (!fp_setup) {
             std::cerr << "Error opening file: " << f_path << std::endl;
             return {std::move(net_conf), std::move(net_top_conf), std::move(print_conf), std::move(sim_conf)};
         }

         std::string s;
         int i = 0;

         while (std::getline(fp_setup, s)) {
             i++;

             // Deleting white space
             s.erase(0, s.find_first_not_of(" \t\n\r"));
             s.erase(s.find_last_not_of(" \t\n\r") + 1);

             // Skip comments
             if (s.empty() || s[0] == '#') continue;

             std::stringstream line(s);
             std::string name, e, value;

             line >> name >> e >> value;

             if (e != "=" || value.empty()) {
                 std::cerr << "Problem with parsing line nr " << i << std::endl;
                 continue;
             }

             try {
                 //Parsing topo_conf
                 if (name == "N_x") {
                     net_top_conf.N_x = std::stoi(value);
                     net_conf.Q_tot = Flow(2 * net_top_conf.N_x);
                     std::cerr << "Setting N_x = " << net_top_conf.N_x << std::endl;
                     std::cerr << "Additionally setting Q_tot = " << net_conf.Q_tot << std::endl;
                 }
                 else if (name == "N_y") {
                    net_top_conf.N_y = std::stoi(value);
                    net_conf.P_in = Pressure (net_top_conf.N_y);
                    std::cerr << "Setting N_x = " << net_top_conf.N_x << std::endl;
                    std::cerr << "Additionally setting P_in = " << net_conf.P_in<< std::endl;
                }

                else if (name == "N_z") {
                     net_top_conf.N_z = std::stoi(value);
                    if (net_top_conf.N_z) net_conf.Q_tot = net_conf.Q_tot * Unitless(net_top_conf.N_z);
                    std::cerr << "Setting N_z = " << net_top_conf.N_z << std::endl;
                    std::cerr << "Additionally setting Q_tot = " << net_conf.Q_tot<< std::endl;
                }

                 else if (name == "type_of_topology") {
                     net_top_conf.type_of_topology = TypeOfNetTopology(std::stoi(value));
                     std::cerr << "Setting net_top_conf.type_of_topology = " << net_top_conf.type_of_topology << std::endl;
                 }
                 else if (name == "in_topology_file_name") {
                     net_top_conf.in_topology_file_name = value;
                     std::cerr << "Setting net_top_conf.in_topology_file_name = " << net_top_conf.in_topology_file_name << std::endl;
                 }
                 else if (name == "in_topology_file_name_g") {
                     net_top_conf.in_topology_file_name_g = value;
                     std::cerr << "Setting net_top_conf.in_topology_file_name_g = " << net_top_conf.in_topology_file_name_g << std::endl;
                 }
                 else if (name == "in_pore_size_file_name") {
                     net_top_conf.in_pore_size_file_name = value;
                     std::cerr << "Setting net_top_conf.in_pore_size_file_name = " << net_top_conf.in_pore_size_file_name << std::endl;
                 }
                 else if (name == "do_radial_geometry") {
                     net_top_conf.do_radial_geometry = (value == "true");
                     std::cerr << "Setting net_top_conf.do_radial_geometry = " << std::boolalpha << net_top_conf.do_radial_geometry << std::endl;
                 }
                 else if (name == "do_randomness_in_regular_net") {
                     net_top_conf.do_randomness_in_regular_net = (value == "true");
                     std::cerr << "Setting net_top_conf.do_randomness_in_regular_net = " << std::boolalpha << net_top_conf.do_randomness_in_regular_net << std::endl;
                 }
                 else if (name == "do_clear_unused_pores") {
                     net_top_conf.do_clear_unused_pores = (value == "true");
                     std::cerr << "Setting net_top_conf.do_clear_unused_pores = " << std::boolalpha << net_top_conf.do_clear_unused_pores << std::endl;
                 }
                 else if (name == "do_periodic_bc") {
                     net_top_conf.do_periodic_bc = (value == "true");
                     std::cerr << "Setting net_top_conf.do_periodic_bc = " << std::boolalpha << net_top_conf.do_periodic_bc << std::endl;
                 }
                 else if (name == "random_seed") {
                     net_top_conf.random_seed = Int(std::stoi(value));
                     std::cerr << "Setting net_top_conf.random_seed = " << net_top_conf.random_seed << std::endl;
                 }
                 else if (name == "gauss_sigma_d") {
                     net_top_conf.gauss_sigma_d = Length(std::stod(value));
                     std::cerr << "Setting net_top_conf.gauss_sigma_d = " << net_top_conf.gauss_sigma_d << std::endl;
                 }
                 else if (name == "max_rand_shift_xy") {
                     net_top_conf.max_rand_shift_xy = Length(std::stod(value));
                     std::cerr << "Setting net_top_conf.max_rand_shift_xy = " << net_top_conf.max_rand_shift_xy << std::endl;
                 }
                 else if (name == "type_of_merging") {
                     net_top_conf.type_of_merging = TypeOfMerging(std::stoi(value));
                     std::cerr << "Setting net_top_conf.type_of_merging = " << net_top_conf.type_of_merging << std::endl;
                 }
                 else if (name == "inlet_cut_factor") {
                     net_top_conf.inlet_cut_factor = Unitless(std::stod(value));
                     std::cerr << "Setting net_top_conf.inlet_cut_factor = " << net_top_conf.inlet_cut_factor << std::endl;
                 }
                 else if (name == "inlet_cut_w") {
                     net_top_conf.inlet_cut_w = Int(std::stoi(value));
                     std::cerr << "Setting net_top_conf.inlet_cut_w = " << net_top_conf.inlet_cut_w << std::endl;
                 }
                 else if (name == "inlet_cut_l") {
                     net_top_conf.inlet_cut_l = Int(std::stoi(value));
                     std::cerr << "Setting net_top_conf.inlet_cut_l = " << net_top_conf.inlet_cut_l << std::endl;
                 }
                 else if (name == "add_well") {
                     net_top_conf.add_well = (value == "true");
                     std::cerr << "Setting net_top_conf.add_well = " << std::boolalpha << net_top_conf.add_well << std::endl;
                 }
                 else if (name == "point_inlet") {
                     net_top_conf.point_inlet = (value == "true");
                     std::cerr << "Setting net_top_conf.point_inlet = " << std::boolalpha << net_top_conf.point_inlet << std::endl;
                 }
                 else if (name == "point_outlet") {
                     net_top_conf.point_outlet = (value == "true");
                     std::cerr << "Setting net_top_conf.point_outlet = " << std::boolalpha << net_top_conf.point_outlet << std::endl;
                 }


                 // Parsing PrintingConfig

                     // Parsing SimulationConfig
                 else if (name == "T_max") {
                     sim_conf.T_max = Long(std::stol(value));
                     std::cerr << "Setting sim_conf.T_max = " << sim_conf.T_max << std::endl;
                 }
                     // Parsing NetworkConfig
                 else if (name == "l0") {
                     net_conf.l0 = Length(std::stod(value));
                     std::cerr << "Setting net_conf.l0 = " << net_conf.l0 << std::endl;
                 }
                 else if (name == "d0") {
                     net_conf.d0 = Length(std::stod(value));
                     std::cerr << "Setting net_conf.d0 = " << net_conf.d0 << std::endl;
                 }
                 else if (name == "Da") {
                     net_conf.Da = Unitless(std::stod(value));
                     std::cerr << "Setting net_conf.Da = " << net_conf.Da << std::endl;
                 }
                 else if (name == "DaPe") {
                     net_conf.DaPe = Unitless(std::stod(value));
                     std::cerr << "Setting net_conf.DaPe = " << net_conf.DaPe << std::endl;
                 }
                 else if (name == "Pe") {
                     net_conf.Pe = Unitless(std::stod(value));
                     std::cerr << "Setting net_conf.Pe = " << net_conf.Pe << std::endl;
                 }
                 else if (name == "Q_tot") {
                     net_conf.Q_tot = Flow(std::stod(value));
                     std::cerr << "Setting net_conf.Q_tot = " << net_conf.Q_tot << std::endl;
                 }
                 else if (name == "P_in") {
                     net_conf.P_in = Pressure(std::stod(value));
                     std::cerr << "Setting net_conf.P_in = " << net_conf.P_in << std::endl;
                 }
                 else if (name == "P_out") {
                     net_conf.P_out = Pressure(std::stod(value));
                     std::cerr << "Setting net_conf.P_out = " << net_conf.P_out << std::endl;
                 }
                 else if (name == "d_min") {
                     net_conf.d_min = Length(std::stod(value));
                     std::cerr << "Setting net_conf.d_min = " << net_conf.d_min << std::endl;
                 }
                 else if (name == "l_min") {
                     net_conf.l_min = Length(std::stod(value));
                     std::cerr << "Setting net_conf.l_min = " << net_conf.l_min << std::endl;
                 }
                 else if (name == "Sh") {
                     net_conf.Sh = Unitless(std::stod(value));
                     std::cerr << "Setting net_conf.Sh = " << net_conf.Sh << std::endl;
                 }
                 else if (name == "mu_0") {
                     net_conf.mu_0 = Viscosity(std::stod(value));
                     std::cerr << "Setting net_conf.mu_0 = " << net_conf.mu_0 << std::endl;
                 }
                 else if (name == "dt_unit") {
                     net_conf.dt_unit = Time(std::stod(value));
                     std::cerr << "Setting net_conf.dt_unit = " << net_conf.dt_unit << std::endl;
                 }


                 //Parsing print_config

                 else if (name == "L_out") {
                     print_conf.L_out = Unitless(std::stod(value));
                     std::cerr << "Setting print_conf.L_out = " << print_conf.L_out << std::endl;
                 }
                 else if (name == "pages_tot") {
                     print_conf.pages_tot = Int(std::stoi(value));
                     std::cerr << "Setting print_conf.pages_tot = " << print_conf.pages_tot << std::endl;
                 }
                 else if (name == "pages_saved") {
                     print_conf.pages_saved = Int(std::stoi(value));
                     std::cerr << "Setting print_conf.pages_saved = " << print_conf.pages_saved << std::endl;
                 }
                 else if (name == "printing_mode") {
                     if (value == "DEBUGGING_PRINT_M")
                         print_conf.printing_mode = PrintingConfig::DEBUGGING_PRINT_M;
                     else if (value == "GRAIN_PRINT_M")
                         print_conf.printing_mode = PrintingConfig::GRAIN_PRINT_M;
                     else
                         std::cerr << "Invalid value for printing_mode in line nr " << i << std::endl;

                     std::cerr << "Setting print_conf.printing_mode = " << print_conf.printing_mode << std::endl;
                 }
                 else if (name == "s_save_data") {
                     print_conf.s_save_data = Int(std::stoi(value));
                     std::cerr << "Setting print_conf.s_save_data = " << print_conf.s_save_data << std::endl;
                 }
                 else if (name == "print_diss_factor") {
                     print_conf.print_diss_factor = Unitless(std::stod(value));
                     std::cerr << "Setting print_conf.print_diss_factor = " << print_conf.print_diss_factor << std::endl;
                 }
                 else if (name == "initial_xy") {
                     print_conf.initial_xy = Int(std::stoi(value));
                     std::cerr << "Setting print_conf.initial_xy = " << print_conf.initial_xy << std::endl;
                 }
                 else if (name == "pattern_anal_factor") {
                     print_conf.pattern_anal_factor = Unitless(std::stod(value));
                     std::cerr << "Setting print_conf.pattern_anal_factor = " << print_conf.pattern_anal_factor << std::endl;
                 }
                 else if (name == "do_save_ps") {
                     if (value == "true")
                         print_conf.do_save_ps = true;
                     else if (value == "false")
                         print_conf.do_save_ps = false;
                     else
                         std::cerr << "WARNING: Wrong value of variable print_conf.do_save_ps. Set true or false." << std::endl;

                     std::cerr << "Setting print_conf.do_save_ps = " << std::boolalpha << print_conf.do_save_ps << std::endl;
                 }
                 else if (name == "do_save_txt") {
                     if (value == "true")
                         print_conf.do_save_txt = true;
                     else if (value == "false")
                         print_conf.do_save_txt = false;
                     else
                         std::cerr << "WARNING: Wrong value of variable print_conf.do_save_txt. Set true or false." << std::endl;

                     std::cerr << "Setting print_conf.do_save_txt = " << std::boolalpha << print_conf.do_save_txt << std::endl;
                 }
                 else if (name == "do_save_table") {
                     if (value == "true")
                         print_conf.do_save_table = true;
                     else if (value == "false")
                         print_conf.do_save_table = false;
                     else
                         std::cerr << "WARNING: Wrong value of variable print_conf.do_save_table. Set true or false." << std::endl;

                     std::cerr << "Setting print_conf.do_save_table = " << std::boolalpha << print_conf.do_save_table << std::endl;
                 }
                 else if (name == "do_save_topology") {
                     if (value == "true")
                         print_conf.do_save_topology = true;
                     else if (value == "false")
                         print_conf.do_save_topology = false;
                     else
                         std::cerr << "WARNING: Wrong value of variable print_conf.do_save_topology. Set true or false." << std::endl;

                     std::cerr << "Setting print_conf.do_save_topology = " << std::boolalpha << print_conf.do_save_topology << std::endl;
                 }
                 else if (name == "do_verbose") {
                     if (value == "true")
                         print_conf.do_verbose = true;
                     else if (value == "false")
                         print_conf.do_verbose = false;
                     else
                         std::cerr << "WARNING: Wrong value of variable print_conf.do_verbose. Set true or false." << std::endl;

                     std::cerr << "Setting print_conf.do_verbose = " << std::boolalpha << print_conf.do_verbose << std::endl;
                 }
                 else if (name == "do_debugging_printing") {
                     if (value == "true")
                         print_conf.do_debugging_printing = true;
                     else if (value == "false")
                         print_conf.do_debugging_printing = false;
                     else
                         std::cerr << "WARNING: Wrong value of variable print_conf.do_debugging_printing. Set true or false." << std::endl;

                     std::cerr << "Setting print_conf.do_debugging_printing = " << std::boolalpha << print_conf.do_debugging_printing << std::endl;
                 }

                 // Parsing Simulation Config
                 else if (name == "T_max") {
                     sim_conf.T_max = Long(std::stol(value));
                     std::cerr << "Setting sim_conf.T_max = " << sim_conf.T_max << std::endl;
                 }
                 else if (name == "tot_steps") {
                     sim_conf.tot_steps = Long(std::stol(value));
                     std::cerr << "Setting sim_conf.tot_steps = " << sim_conf.tot_steps << std::endl;
                 }
                 else if (name == "tot_time") {
                     sim_conf.tot_time = Time(std::stod(value));
                     std::cerr << "Setting sim_conf.tot_time = " << sim_conf.tot_time << std::endl;
                 }
                 else if (name == "d_d_max") {
                     sim_conf.d_d_max = Length(std::stod(value));
                     std::cerr << "Setting sim_conf.d_d_max = " << sim_conf.d_d_max << std::endl;
                 }
                 else if (name == "d_d_min") {
                     sim_conf.d_d_min = Length(std::stod(value));
                     std::cerr << "Setting sim_conf.d_d_min = " << sim_conf.d_d_min << std::endl;
                 }
                 else if (name == "d_d_dis") {
                     sim_conf.d_d_dis = Unitless(std::stod(value));
                     std::cerr << "Setting sim_conf.d_d_dis = " << sim_conf.d_d_dis << std::endl;
                 }
                 else if (name == "u_min") {
                     sim_conf.u_min = Pressure(std::stod(value));
                     std::cerr << "Setting sim_conf.u_min = " << sim_conf.u_min << std::endl;
                 }
                 else if (name == "d_V_min") {
                     sim_conf.d_V_min = Volume(std::stod(value));
                     std::cerr << "Setting sim_conf.d_V_min = " << sim_conf.d_V_min << std::endl;
                 }
                 else if (name == "d_V_max") {
                     sim_conf.d_V_max = Volume(std::stod(value));
                     std::cerr << "Setting sim_conf.d_V_max = " << sim_conf.d_V_max << std::endl;
                 }
                 else if (name == "time_factor_after_b") {
                     sim_conf.time_factor_after_b = Unitless(std::stod(value));
                     std::cerr << "Setting sim_conf.time_factor_after_b = " << sim_conf.time_factor_after_b << std::endl;
                 }
                 else if (name == "do_leapfrog") {
                     if (value == "true")
                         sim_conf.do_leapfrog = true;
                     else if (value == "false")
                         sim_conf.do_leapfrog = false;
                     else
                         std::cerr << "WARNING: Wrong value of variable sim_conf.do_leapfrog. Set true or false." << std::endl;

                     std::cerr << "Setting sim_conf.do_leapfrog = " << std::boolalpha << sim_conf.do_leapfrog << std::endl;
                 }
                 else if (name == "do_full_dissolution") {
                     if (value == "true")
                         sim_conf.do_full_dissolution = true;
                     else if (value == "false")
                         sim_conf.do_full_dissolution = false;
                     else
                         std::cerr << "WARNING: Wrong value of variable sim_conf.do_full_dissolution. Set true or false." << std::endl;

                     std::cerr << "Setting sim_conf.do_full_dissolution = " << std::boolalpha << sim_conf.do_full_dissolution << std::endl;
                 }
                 else if (name == "do_system_dissolved") {
                     if (value == "true")
                         sim_conf.do_system_dissolved = true;
                     else if (value == "false")
                         sim_conf.do_system_dissolved = false;
                     else
                         std::cerr << "WARNING: Wrong value of variable sim_conf.do_system_dissolved. Set true or false." << std::endl;

                     std::cerr << "Setting sim_conf.do_system_dissolved = " << std::boolalpha << sim_conf.do_system_dissolved << std::endl;
                 }
                 else if (name == "do_adaptive_dt") {
                     if (value == "true")
                         sim_conf.do_adaptive_dt = true;
                     else if (value == "false")
                         sim_conf.do_adaptive_dt = false;
                     else
                         std::cerr << "WARNING: Wrong value of variable sim_conf.do_adaptive_dt. Set true or false." << std::endl;

                     std::cerr << "Setting sim_conf.do_adaptive_dt = " << std::boolalpha << sim_conf.do_adaptive_dt << std::endl;
                 }
                 else if (name == "do_recalculate_physical_parameters") {
                     if (value == "true")
                         sim_conf.do_recalculate_physical_parameters = true;
                     else if (value == "false")
                         sim_conf.do_recalculate_physical_parameters = false;
                     else
                         std::cerr << "WARNING: Wrong value of variable sim_conf.do_recalculate_physical_parameters. Set true or false." << std::endl;

                     std::cerr << "Setting sim_conf.do_recalculate_physical_parameters = " << std::boolalpha << sim_conf.do_recalculate_physical_parameters << std::endl;
                 }
                 else if (name == "do_smarter_calculation_of_pressure") {
                     if (value == "true")
                         sim_conf.do_smarter_calculation_of_pressure = true;
                     else if (value == "false")
                         sim_conf.do_smarter_calculation_of_pressure = false;
                     else
                         std::cerr << "WARNING: Wrong value of variable sim_conf.do_smarter_calculation_of_pressure. Set true or false." << std::endl;

                     std::cerr << "Setting sim_conf.do_smarter_calculation_of_pressure = " << std::boolalpha << sim_conf.do_smarter_calculation_of_pressure << std::endl;
                 }
                 else if (name == "do_dynamical_length") {
                     if (value == "true")
                         sim_conf.do_dynamical_length = true;
                     else if (value == "false")
                         sim_conf.do_dynamical_length = false;
                     else
                         std::cerr << "WARNING: Wrong value of variable sim_conf.do_dynamical_length. Set true or false." << std::endl;

                     std::cerr << "Setting sim_conf.do_dynamical_length = " << std::boolalpha << sim_conf.do_dynamical_length << std::endl;
                 }
                 else if (name == "do_cut_d_min") {
                     if (value == "true")
                         sim_conf.do_cut_d_min = true;
                     else if (value == "false")
                         sim_conf.do_cut_d_min = false;
                     else
                         std::cerr << "WARNING: Wrong value of variable sim_conf.do_cut_d_min. Set true or false." << std::endl;

                     std::cerr << "Setting sim_conf.do_cut_d_min = " << std::boolalpha << sim_conf.do_cut_d_min << std::endl;
                 }
                 else if (name == "d_max_for_u") {
                     sim_conf.d_max_for_u = Length(std::stod(value));
                     std::cerr << "Setting sim_conf.d_max_for_u = " << sim_conf.d_max_for_u << std::endl;
                 }

                //Parsing tables (maps)

                 else {
                     // Handling std::map<SPECIES,Unitless> gamma and similar maps
                     if (name.find("gamma[") == 0) {
                         auto species = name.substr(6, name.size() - 7); // Extracting species
                         net_conf.gamma[SPECIES(species)] = Unitless(std::stod(value));
                         std::cerr << "Setting net_conf.gamma[" << species << "] = " << net_conf.gamma[SPECIES(species)] << std::endl;
                     }
                     else if (name.find("kappa[") == 0) {
                         auto reaction = name.substr(6, name.size() - 7); // Extracting reaction
                         net_conf.kappa[ChemicalReaction(reaction)] = Unitless(std::stod(value));
                         std::cerr << "Setting net_conf.kappa[" << reaction << "] = " << net_conf.kappa[ChemicalReaction(reaction)] << std::endl;
                     }
                     else if (name.find("theta[") == 0) {
                         auto reaction = name.substr(6, name.size() - 7); // Extracting reaction
                         net_conf.theta[ChemicalReaction(reaction)] = Unitless(std::stod(value));
                         std::cerr << "Setting net_conf.theta[" << reaction << "] = " << net_conf.theta[ChemicalReaction(reaction)] << std::endl;
                     }
                     else if (name.find("lambda[") == 0) {
                         auto reaction = name.substr(7, name.size() - 8); // Extracting reaction
                         net_conf.lambda[ChemicalReaction(reaction)] = Unitless(std::stod(value));
                         std::cerr << "Setting net_conf.lambda[" << reaction << "] = " << net_conf.lambda[ChemicalReaction(reaction)] << std::endl;
                     }
                     else if (name.find("reaction_rate[") == 0) {
                         auto reaction = name.substr(15, name.size() - 16); // Extracting reaction
                         net_conf.reaction_rate[ChemicalReaction(reaction)] = Velocity(std::stod(value));
                         std::cerr << "Setting net_conf.reaction_rate[" << reaction << "] = " << net_conf.reaction_rate[ChemicalReaction(reaction)] << std::endl;
                     }
                     else if (name.find("diffusion_rate[") == 0) {
                         auto species = name.substr(15, name.size() - 16); // Extracting species
                         net_conf.diffusion_rate[SPECIES(species)] = Diffusion(std::stod(value));
                         std::cerr << "Setting net_conf.diffusion_rate[" << species << "] = " << net_conf.diffusion_rate[SPECIES(species)] << std::endl;
                     }
                     else if (name.find("transversal_diffusion_rate[") == 0) {
                         auto species = name.substr(27, name.size() - 28); // Extracting species
                         net_conf.transversal_diffusion_rate[SPECIES(species)] = Diffusion(std::stod(value));
                         std::cerr << "Setting net_conf.transversal_diffusion_rate[" << species << "] = " << net_conf.transversal_diffusion_rate[SPECIES(species)] << std::endl;
                     }
                     else if (name.find("inlet_c[") == 0) {
                         auto species = name.substr(8, name.size() - 9); // Extracting species
                         net_conf.inlet_c[SPECIES(species)] = Concentration(std::stod(value));
                         std::cerr << "Setting net_conf.inlet_c[" << species << "] = " << net_conf.inlet_c[SPECIES(species)] << std::endl;
                     }

                     else {
                         std::cerr << "Unknown configuration variable " << name << " in line nr " << i << std::endl;
                     }
                 }
             } catch (const std::invalid_argument& ia) {
                 std::cerr << "Invalid value for " << name << " in line nr " << i << std::endl;
             }


//             if (name == "L_out") {
//                 try {
//                     print_conf.L_out = Unitless(std::stod(value));
//                     std::cerr << "Setting print_conf.L_out = " << print_conf.L_out << std::endl;
//                 } catch (const std::invalid_argument& ia) {
//                     std::cerr << "Invalid value for L_out in line nr " << i << std::endl;
//                 }
//             }
//
//             else if (name == "do_save_ps") {
//                 if      (value == "true" )   print_conf.do_save_ps = true;
//                 else if (value == "false")   print_conf.do_save_ps = false;
//                 else    std::cerr<<"WARNING: Wrong value of variable print_conf.do_save_ps. Set true or false."<<std::endl;
//
//                 std::cerr<< "Setting print_conf.do_save_ps  = "<<print_conf.do_save_ps  <<std::endl;
//             }
//
//
//             else std::cerr << "Unknown variable in line nr " << i << std::endl;
//
//


        return {std::move(net_conf), std::move(topo_conf), std::move(print_conf), std::move(sym_conf)};
    }

}
#endif //KARST_4_0_READ_CONFIG_H

