//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_READ_CONFIG_H
#define KARST_4_0_READ_CONFIG_H

#include <functional>
#include "src/utils.h"
#include "src/units.h"
#include "src/network/NetworkConfig.h"
#include "src/network/NetworkTopologyConfig.h"
#include "src/simulation//SimulationConfig.h"
#include "src/import_export/PrintingConfig.h"


namespace karst {

     inline auto read_configs(const std::string& f_path, Logger<logger_level_min>& log) -> std::tuple<NetworkConfig, NetworkTopologyConfig, PrintingConfig, SimulationConfig> {

         log.log<LogLevel::INFO>("Parsing the configuration file...");

        // Creating all configs
        NetworkConfig           net_conf        {};
        NetworkTopologyConfig   net_top_conf    {};
        PrintingConfig          print_conf      {};
        SimulationConfig        sim_conf        {};


        // Parser config map
        //TODO ; przepisać ten kawałek tak, by mieć tylko mapę łączątą stringi ze wskaźnikami oraz dla każdego typu przeładowanie typ+ string!!!

        const std::map<std::string, std::function<void(const std::string&)>> config_map = {
                 {"N_x", [&](const std::string& value) {
                     net_top_conf.N_x = std::stoi(value);
                     net_conf.Q_tot = Flow(2 * net_top_conf.N_x);
                     log.log<LogLevel::INFO>("Setting N_x = " + std::to_string(net_top_conf.N_x));
                     log.log<LogLevel::INFO>("Additionally setting Q_tot = " + net_conf.Q_tot);
                 }},
                 {"N_y", [&](const std::string& value) {
                     net_top_conf.N_y = std::stoi(value);
                     net_conf.P_in = Pressure(net_top_conf.N_y);
                     log.log<LogLevel::INFO>("Setting N_y = " + std::to_string(net_top_conf.N_y));
                     log.log<LogLevel::INFO>("Additionally setting P_in = " + net_conf.P_in);
                 }},
                 {"N_z", [&](const std::string& value) {
                     net_top_conf.N_z = std::stoi(value);
                     if (net_top_conf.N_z) net_conf.Q_tot = net_conf.Q_tot * Unitless(net_top_conf.N_z);
                     log.log<LogLevel::INFO>("Setting N_z = " + std::to_string(net_top_conf.N_z));
                     log.log<LogLevel::INFO>("Additionally setting Q_tot = " + net_conf.Q_tot);
                 }},
                 {"type_of_topology", [&](const std::string& value) {
                     net_top_conf.type_of_topology+value;
                     log.log<LogLevel::INFO>("Setting net_top_conf.type_of_topology = " + net_top_conf.type_of_topology);
                 }},
                 {"in_topology_file_name", [&](const std::string& value) {
                     net_top_conf.in_topology_file_name = value;
                     log.log<LogLevel::INFO>("Setting net_top_conf.in_topology_file_name = " + net_top_conf.in_topology_file_name);
                 }},
                 {"in_topology_file_name_g", [&](const std::string& value) {
                     net_top_conf.in_topology_file_name_g = value;
                     log.log<LogLevel::INFO>("Setting net_top_conf.in_topology_file_name_g = " + net_top_conf.in_topology_file_name_g);
                 }},
                 {"in_pore_size_file_name", [&](const std::string& value) {
                     net_top_conf.in_pore_size_file_name = value;
                     log.log<LogLevel::INFO>("Setting net_top_conf.in_pore_size_file_name = " + net_top_conf.in_pore_size_file_name);
                 }},
                 {"do_radial_geometry", [&](const std::string& value) {
                     net_top_conf.do_radial_geometry = (value == "true");
                     log.log<LogLevel::INFO>("Setting net_top_conf.do_radial_geometry = " + bool_to_string(net_top_conf.do_radial_geometry));
                 }},
                 {"do_randomness_in_regular_net", [&](const std::string& value) {
                     net_top_conf.do_randomness_in_regular_net = (value == "true");
                     log.log<LogLevel::INFO>("Setting net_top_conf.do_randomness_in_regular_net = " + bool_to_string(net_top_conf.do_randomness_in_regular_net));
                 }},
                 {"do_clear_unused_pores", [&](const std::string& value) {
                     net_top_conf.do_clear_unused_net_el = (value == "true");
                     log.log<LogLevel::INFO>("Setting net_top_conf.do_clear_unused_pores = " + bool_to_string(net_top_conf.do_clear_unused_net_el));
                 }},
                 {"do_periodic_bc", [&](const std::string& value) {
                     net_top_conf.do_periodic_bc = (value == "true");
                     log.log<LogLevel::INFO>("Setting net_top_conf.do_periodic_bc = " + bool_to_string(net_top_conf.do_periodic_bc));
                 }},
                 {"random_seed", [&](const std::string& value) {
                     net_top_conf.random_seed = Int(std::stoi(value));
                     log.log<LogLevel::INFO>("Setting net_top_conf.random_seed = " + std::to_string(net_top_conf.random_seed));
                 }},
                 {"gauss_sigma_d", [&](const std::string& value) {
                     net_top_conf.gauss_sigma_d = Length(std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_top_conf.gauss_sigma_d = " + net_top_conf.gauss_sigma_d);
                 }},
                 {"max_rand_shift_xy", [&](const std::string& value) {
                     net_top_conf.max_rand_shift = Unitless (std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_top_conf.max_rand_shift_xy = " + net_top_conf.max_rand_shift);
                 }},
                 {"type_of_merging", [&](const std::string& value) {
                     net_top_conf.type_of_merging + value;
                     log.log<LogLevel::INFO>("Setting net_top_conf.type_of_merging = " + net_top_conf.type_of_merging);
                 }},
                 {"inlet_cut_factor", [&](const std::string& value) {
                     net_top_conf.inlet_cut_factor = Unitless(std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_top_conf.inlet_cut_factor = " + net_top_conf.inlet_cut_factor);
                 }},
                 {"inlet_cut_w", [&](const std::string& value) {
                     net_top_conf.inlet_cut_w = Int(std::stoi(value));
                     log.log<LogLevel::INFO>("Setting net_top_conf.inlet_cut_w = " + std::to_string(net_top_conf.inlet_cut_w));
                 }},
                 {"inlet_cut_l", [&](const std::string& value) {
                     net_top_conf.inlet_cut_l = Int(std::stoi(value));
                     log.log<LogLevel::INFO>("Setting net_top_conf.inlet_cut_l = " + std::to_string(net_top_conf.inlet_cut_l));
                 }},
                 {"point_inlet", [&](const std::string& value) {
                     net_top_conf.point_inlet = (value == "true");
                     log.log<LogLevel::INFO>("Setting net_top_conf.point_inlet = " + bool_to_string(net_top_conf.point_inlet));
                 }},
                 {"point_outlet", [&](const std::string& value) {
                     net_top_conf.point_outlet = (value == "true");
                     log.log<LogLevel::INFO>("Setting net_top_conf.point_outlet = " + bool_to_string(net_top_conf.point_outlet));
                 }},
                 {"T_max", [&](const std::string& value) {
                     sim_conf.T_max = Time(std::stod(value));
                     log.log<LogLevel::INFO>("Setting sim_conf.T_max = " + sim_conf.T_max);
                 }},
                 {"l0", [&](const std::string& value) {
                     net_conf.l0 = Length(std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_conf.l0 = " + net_conf.l0);
                 }},
                 {"d0", [&](const std::string& value) {
                     net_conf.d0 = Length(std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_conf.d0 = " + net_conf.d0);
                 }},
                 {"Da", [&](const std::string& value) {
                     net_conf.Da = Unitless(std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_conf.Da = " + net_conf.Da);
                 }},
                 {"DaPe", [&](const std::string& value) {
                     net_conf.DaPe = Unitless(std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_conf.DaPe = " + net_conf.DaPe);
                 }},
                 {"Pe", [&](const std::string& value) {
                     net_conf.Pe = Unitless(std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_conf.Pe = " + net_conf.Pe);
                 }},
                 {"Q_tot", [&](const std::string& value) {
                     net_conf.Q_tot = Flow(std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_conf.Q_tot = " + net_conf.Q_tot);
                 }},
                 {"P_in", [&](const std::string& value) {
                     net_conf.P_in = Pressure(std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_conf.P_in = " + net_conf.P_in);
                 }},
                 {"P_out", [&](const std::string& value) {
                     net_conf.P_out = Pressure(std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_conf.P_out = " + net_conf.P_out);
                 }},
                 {"d_min", [&](const std::string& value) {
                     net_conf.d_min = Length(std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_conf.d_min = " + net_conf.d_min);
                 }},
                 {"l_min", [&](const std::string& value) {
                     net_conf.l_min = Length(std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_conf.l_min = " + net_conf.l_min);
                 }},
                 {"Sh", [&](const std::string& value) {
                     net_conf.Sh = Unitless(std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_conf.Sh = " + net_conf.Sh);
                 }},
                 {"mu_0", [&](const std::string& value) {
                     net_conf.mu_0 = Viscosity(std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_conf.mu_0 = " + net_conf.mu_0);
                 }},
                 {"dt_unit", [&](const std::string& value) {
                     net_conf.dt_unit = Time(std::stod(value));
                     log.log<LogLevel::INFO>("Setting net_conf.dt_unit = " + net_conf.dt_unit);
                 }},
                 {"L_out", [&](const std::string& value) {
                     print_conf.L_out = Unitless(std::stod(value));
                     log.log<LogLevel::INFO>("Setting print_conf.L_out = " + print_conf.L_out);
                 }},
                 {"pages_tot", [&](const std::string& value) {
                     print_conf.pages_tot = Int(std::stoi(value));
                     log.log<LogLevel::INFO>("Setting print_conf.pages_tot = " + std::to_string(print_conf.pages_tot));
                 }},
                 {"pages_saved", [&](const std::string& value) {
                     print_conf.pages_saved = Int(std::stoi(value));
                     log.log<LogLevel::INFO>("Setting print_conf.pages_saved = " + std::to_string(print_conf.pages_saved));
                 }},
                 {"printing_mode", [&](const std::string& value) {
                     print_conf.printing_mode+value;
                     log.log<LogLevel::INFO>("Setting print_conf.printing_mode = " + print_conf.printing_mode);
                 }},
                 {"s_save_data", [&](const std::string& value) {
                     print_conf.s_save_data = Int(std::stoi(value));
                     log.log<LogLevel::INFO>("Setting print_conf.s_save_data = " + std::to_string(print_conf.s_save_data));
                 }},
                 {"print_diss_factor", [&](const std::string& value) {
                     print_conf.print_diss_factor = Unitless(std::stod(value));
                     log.log<LogLevel::INFO>("Setting print_conf.print_diss_factor = " + print_conf.print_diss_factor);
                 }},
                 {"initial_xy", [&](const std::string& value) {
                     print_conf.initial_xy = Int(std::stoi(value));
                     log.log<LogLevel::INFO>("Setting print_conf.initial_xy = " + std::to_string(print_conf.initial_xy));
                 }},
                 {"pattern_anal_factor", [&](const std::string& value) {
                     print_conf.pattern_anal_factor = Unitless(std::stod(value));
                     log.log<LogLevel::INFO>("Setting print_conf.pattern_anal_factor = " + print_conf.pattern_anal_factor);
                 }},
                 {"do_save_ps", [&](const std::string& value) {
                     if (value == "true")
                         print_conf.do_save_ps = true;
                     else if (value == "false")
                         print_conf.do_save_ps = false;
                     else
                         log.log<LogLevel::INFO>("WARNING: Wrong value of variable print_conf.do_save_ps. Set true or false.");
                     log.log<LogLevel::INFO>("Setting print_conf.do_save_ps = " + bool_to_string(print_conf.do_save_ps));
                 }},
                 {"do_save_txt", [&](const std::string& value) {
                     if (value == "true")
                         print_conf.do_save_txt = true;
                     else if (value == "false")
                         print_conf.do_save_txt = false;
                     else
                         log.log<LogLevel::INFO>("WARNING: Wrong value of variable print_conf.do_save_txt. Set true or false.");
                     log.log<LogLevel::INFO>("Setting print_conf.do_save_txt = " + bool_to_string(print_conf.do_save_txt));
                 }},
                 {"do_save_table", [&](const std::string& value) {
                     if (value == "true")
                         print_conf.do_save_table = true;
                     else if (value == "false")
                         print_conf.do_save_table = false;
                     else
                         log.log<LogLevel::INFO>("WARNING: Wrong value of variable print_conf.do_save_table. Set true or false.");
                     log.log<LogLevel::INFO>("Setting print_conf.do_save_table = " + bool_to_string(print_conf.do_save_table));
                 }},
                 {"do_save_topology", [&](const std::string& value) {
                     if (value == "true")
                         print_conf.do_save_topology = true;
                     else if (value == "false")
                         print_conf.do_save_topology = false;
                     else
                         log.log<LogLevel::INFO>("WARNING: Wrong value of variable print_conf.do_save_topology. Set true or false.");
                     log.log<LogLevel::INFO>("Setting print_conf.do_save_topology = " + bool_to_string(print_conf.do_save_topology));
                 }},
                 {"do_verbose", [&](const std::string& value) {
                     if (value == "true")
                         print_conf.do_verbose = true;
                     else if (value == "false")
                         print_conf.do_verbose = false;
                     else
                         log.log<LogLevel::INFO>("WARNING: Wrong value of variable print_conf.do_verbose. Set true or false.");
                     log.log<LogLevel::INFO>("Setting print_conf.do_verbose = " + bool_to_string(print_conf.do_verbose));
                 }},
                 {"do_debugging_printing", [&](const std::string& value) {
                     if (value == "true")
                         print_conf.do_debugging_printing = true;
                     else if (value == "false")
                         print_conf.do_debugging_printing = false;
                     else
                         log.log<LogLevel::INFO>("WARNING: Wrong value of variable print_conf.do_debugging_printing. Set true or false.");
                     log.log<LogLevel::INFO>("Setting print_conf.do_debugging_printing = " + bool_to_string(print_conf.do_debugging_printing));
                 }},
                 {"tot_steps", [&](const std::string& value) {
                     sim_conf.tot_steps = Long(std::stol(value));
                     log.log<LogLevel::INFO>("Setting sim_conf.tot_steps = " + std::to_string(sim_conf.tot_steps));
                 }},
                 {"tot_time", [&](const std::string& value) {
                     sim_conf.tot_time = Time(std::stod(value));
                     log.log<LogLevel::INFO>("Setting sim_conf.tot_time = " + sim_conf.tot_time);
                 }},
                 {"d_d_max", [&](const std::string& value) {
                     sim_conf.d_d_max = Length(std::stod(value));
                     log.log<LogLevel::INFO>("Setting sim_conf.d_d_max = " + sim_conf.d_d_max);
                 }},
                 {"d_d_min", [&](const std::string& value) {
                     sim_conf.d_d_min = Length(std::stod(value));
                     log.log<LogLevel::INFO>("Setting sim_conf.d_d_min = " + sim_conf.d_d_min);
                 }},
                 {"d_d_dis", [&](const std::string& value) {
                     sim_conf.d_d_dis = Unitless(std::stod(value));
                     log.log<LogLevel::INFO>("Setting sim_conf.d_d_dis = " + sim_conf.d_d_dis);
                 }},
                 {"u_min", [&](const std::string& value) {
                     sim_conf.u_min = Pressure(std::stod(value));
                     log.log<LogLevel::INFO>("Setting sim_conf.u_min = " + sim_conf.u_min);
                 }},
                 {"d_V_min", [&](const std::string& value) {
                     sim_conf.d_V_min = Volume(std::stod(value));
                     log.log<LogLevel::INFO>("Setting sim_conf.d_V_min = " + sim_conf.d_V_min);
                 }},
                 {"d_V_max", [&](const std::string& value) {
                     sim_conf.d_V_max = Volume(std::stod(value));
                     log.log<LogLevel::INFO>("Setting sim_conf.d_V_max = " + sim_conf.d_V_max);
                 }},
                 {"time_factor_after_b", [&](const std::string& value) {
                     sim_conf.time_factor_after_b = Unitless(std::stod(value));
                     log.log<LogLevel::INFO>("Setting sim_conf.time_factor_after_b = " + sim_conf.time_factor_after_b);
                 }},
                 {"do_leapfrog", [&](const std::string& value) {
                     if (value == "true")
                         sim_conf.do_leapfrog = true;
                     else if (value == "false")
                         sim_conf.do_leapfrog = false;
                     else
                         log.log<LogLevel::INFO>("WARNING: Wrong value of variable sim_conf.do_leapfrog. Set true or false.");

                     log.log<LogLevel::INFO>("Setting sim_conf.do_leapfrog = " + bool_to_string(sim_conf.do_leapfrog));
                 }},
                 {"do_full_dissolution", [&](const std::string& value) {
                     if (value == "true")
                         sim_conf.do_full_dissolution = true;
                     else if (value == "false")
                         sim_conf.do_full_dissolution = false;
                     else
                         log.log<LogLevel::INFO>("WARNING: Wrong value of variable sim_conf.do_full_dissolution. Set true or false.");

                     log.log<LogLevel::INFO>("Setting sim_conf.do_full_dissolution = " + bool_to_string(sim_conf.do_full_dissolution));
                 }},
                 {"do_system_dissolved", [&](const std::string& value) {
                     if (value == "true")
                         sim_conf.do_system_dissolved = true;
                     else if (value == "false")
                         sim_conf.do_system_dissolved = false;
                     else
                         log.log<LogLevel::INFO>("WARNING: Wrong value of variable sim_conf.do_system_dissolved. Set true or false.");

                     log.log<LogLevel::INFO>("Setting sim_conf.do_system_dissolved = " + bool_to_string(sim_conf.do_system_dissolved));
                 }},
                 {"do_adaptive_dt", [&](const std::string& value) {
                     if (value == "true")
                         sim_conf.do_adaptive_dt = true;
                     else if (value == "false")
                         sim_conf.do_adaptive_dt = false;
                     else
                         log.log<LogLevel::INFO>("WARNING: Wrong value of variable sim_conf.do_adaptive_dt. Set true or false.");

                     log.log<LogLevel::INFO>("Setting sim_conf.do_adaptive_dt = " + bool_to_string(sim_conf.do_adaptive_dt));
                 }},
                 {"do_recalculate_physical_parameters", [&](const std::string& value) {
                     if (value == "true")
                         sim_conf.do_recalculate_physical_parameters = true;
                     else if (value == "false")
                         sim_conf.do_recalculate_physical_parameters = false;
                     else
                         log.log<LogLevel::INFO>("WARNING: Wrong value of variable sim_conf.do_recalculate_physical_parameters. Set true or false.");

                     log.log<LogLevel::INFO>("Setting sim_conf.do_recalculate_physical_parameters = " + bool_to_string(sim_conf.do_recalculate_physical_parameters));
                 }},
                 {"do_smarter_calculation_of_pressure", [&](const std::string& value) {
                     std::stringstream (value) >> std::boolalpha >> sim_conf.do_smarter_calculation_of_pressure;
                     log.log<LogLevel::INFO>("Setting sim_conf.do_smarter_calculation_of_pressure = " + bool_to_string(sim_conf.do_smarter_calculation_of_pressure));
                 }},
                 {"do_dynamical_length", [&](const std::string& value) {
                     std::stringstream (value) >> std::boolalpha >> sim_conf.do_dynamical_length;
                     log.log<LogLevel::INFO>("Setting sim_conf.do_dynamical_length = " + bool_to_string(sim_conf.do_dynamical_length));
                 }},
                 {"do_cut_d_min", [&](const std::string& value) {
                     std::stringstream (value) >> std::boolalpha >> sim_conf.do_cut_d_min;
                     log.log<LogLevel::INFO>("Setting sim_conf.do_cut_d_min = " + bool_to_string(sim_conf.do_cut_d_min));
                 }},
                 {"d_max_for_u", [&](const std::string& value) {
                     sim_conf.d_max_for_u = Length(std::stod(value));
                     log.log<LogLevel::INFO>("Setting sim_conf.d_max_for_u = " + sim_conf.d_max_for_u);
                 }},

                 //TODO: Wykorzystać poniżej :    std::regex pattern("Hello.*");  if (std::regex_match(str, pattern)) {


//                 {"gamma[", [&](const std::string& value) {
//                     auto species = name.substr(6, name.size() - 7);
//                     net_conf.gamma[SPECIES(species)] = Unitless(std::stod(value));
//                     log.log<LogLevel::INFO>("Setting net_conf.gamma[" + species + "] = " + net_conf.gamma[SPECIES(species)]);
//                 }},
//                 {"kappa[", [&](const std::string& value) {
//                     auto reaction = name.substr(6, name.size() - 7);
//                     net_conf.kappa[ChemicalReaction(reaction)] = Unitless(std::stod(value));
//                     log.log<LogLevel::INFO>("Setting net_conf.kappa[" + reaction + "] = " + net_conf.kappa[ChemicalReaction(reaction)]);
//                 }},
//                 {"theta[", [&](const std::string& value) {
//                     auto reaction = name.substr(6, name.size() - 7);
//                     net_conf.theta[ChemicalReaction(reaction)] = Unitless(std::stod(value));
//                     log.log<LogLevel::INFO>("Setting net_conf.theta[" + reaction + "] = " + net_conf.theta[ChemicalReaction(reaction)]);
//                 }},
//                 {"lambda[", [&](const std::string& value) {
//                     auto reaction = name.substr(7, name.size() - 8);
//                     net_conf.lambda[ChemicalReaction(reaction)] = Unitless(std::stod(value));
//                     log.log<LogLevel::INFO>("Setting net_conf.lambda[" + reaction + "] = " + net_conf.lambda[ChemicalReaction(reaction)]);
//                 }},
//                 {"reaction_rate[", [&](const std::string& value) {
//                     auto reaction = name.substr(15, name.size() - 16);
//                     net_conf.reaction_rate[ChemicalReaction(reaction)] = Velocity(std::stod(value));
//                     log.log<LogLevel::INFO>("Setting net_conf.reaction_rate[" + reaction + "] = " + net_conf.reaction_rate[ChemicalReaction(reaction)]);
//                 }},
//                 {"diffusion_rate[", [&](const std::string& value) {
//                     auto species = name.substr(15, name.size() - 16);
//                     net_conf.diffusion_rate[SPECIES(species)] = Diffusion(std::stod(value));
//                     log.log<LogLevel::INFO>("Setting net_conf.diffusion_rate[" + species + "] = " + net_conf.diffusion_rate[SPECIES(species)]);
//                 }},
//                 {"transversal_diffusion_rate[", [&](const std::string& value) {
//                     auto species = name.substr(27, name.size() - 28);
//                     net_conf.transversal_diffusion_rate[SPECIES(species)] = Diffusion(std::stod(value));
//                     log.log<LogLevel::INFO>("Setting net_conf.transversal_diffusion_rate[" + species + "] = " + net_conf.transversal_diffusion_rate[SPECIES(species)]);
//                 }},
//                 {"inlet_c[", [&](const std::string& value) {
//                     auto species = name.substr(8, name.size() - 9);
//                     net_conf.inlet_c[SPECIES(species)] = Concentration(std::stod(value));
//                     log.log<LogLevel::INFO>("Setting net_conf.inlet_c[" + species + "] = " + net_conf.inlet_c[SPECIES(species)]);
//                 }}
         };



         // reading config file:
         std::ifstream fp_setup(f_path);
         if (!fp_setup) {
             log.log<LogLevel::ERROR>("Error opening file: " + f_path);
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
                 log.log<LogLevel::WARNING>("Problem with parsing line nr " + std::to_string(i));
                 continue;
             }


             // Search for a proper entry in config_map
             if (config_map.find(name) != config_map.end()) {

                 try {
                     auto& func = config_map.at(name); // Pobierz referencję do funkcji za pomocą at(), aby sprawdzić, czy klucz istnieje
                     func(value); // Wywołaj funkcję z parametrem value
                 }
                 catch (const std::invalid_argument &ia) {
                     log.log<LogLevel::WARNING>("Invalid value for " + name + " in line nr " + std::to_string(i));
                 }
                 catch (const std::bad_function_call &bfc) {
                     log.log<LogLevel::WARNING>("Error calling function for key " + name + ": " + bfc.what());
                 }
             }
                 //TODO: dopisać tutaj elseif dla mapy z parsowaniem wyrażeń regularnych!!!
             else {
                 log.log<LogLevel::WARNING>("Problem with parsing line nr " +std::to_string(i));
                 log.log<LogLevel::WARNING>("Key " + name + " not found in config_map.");
             }
         }

        return {std::move(net_conf), std::move(net_top_conf), std::move(print_conf), std::move(sim_conf)};
    }

}
#endif //KARST_4_0_READ_CONFIG_H

