//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_PRINTINGMODULE_H
#define KARST_4_0_PRINTINGMODULE_H


#include "src/utils.h"
#include "src/units.h"
#include "PrintingConfig.h"
#include "src/network/NetworkConfig.h"
#include "src/network/NetworkTopologyConfig.h"
#include "src/import_export/printing_primitives.h"
#include "src/import_export/printing_net_elements.h"
#include "src/all_enums.h"


namespace karst {


    class PrintingModule {

        struct PrintingModuleState {
            bool print_txt_now;
            bool print_ps_now;
            bool print_vtk_now;
        };

    public:

        friend Network;

        explicit PrintingModule(const NetworkConfig &net_conf0,
                                const NetworkTopologyConfig &net_topo_conf0,
                                const PrintingConfig &config0,
                                Logger<logger_level_min> &log0) :
                net_topo_config{net_topo_conf0},
                net_config{net_conf0},
                config{config0},
                log{log0} {


            time_evolution_out.open("time_evolution.out");
            if (config.do_save_txt) {
                d_out.init();
                q_out.init();
                u_out.init();
                l_out.init();
                for (auto s: net_config.solubleS) C_out[s]->init();
                for (auto s: net_config.solidS)   V_out[s]->init();
            }
            if (config.do_save_ps) {
                net_pores_ps.init();
                net_grains_ps.init();
                if (logger_level_min == LogLevel::DEBUG_PS)
                    net_debug_ps.init();
            }
            if (config.do_save_vtk) {
                net_out.init();
                pore_nodes_out.init();
            }

        };


        ~PrintingModule() {
            time_evolution_out.close();
            if (config.do_save_txt) {
                d_out.close();
                q_out.close();
                u_out.close();
                l_out.close();
                for (auto s: net_config.solubleS) C_out[s]->close();
                for (auto s: net_config.solidS)   V_out[s]->close();
            }
            if (config.do_save_ps) {
                net_pores_ps.close();
                net_grains_ps.close();
                if (logger_level_min == LogLevel::DEBUG_PS)
                    net_debug_ps.close();
            }
            if (config.do_save_vtk) {
                net_out.close();
                pore_nodes_out.close();
            }
        }


        void print_ps_headlines(OutStream &stream, int page_nr, const std::string &description_note);

        void print_net_ps(const std::string title,
                          const std::vector<Node> &nodes,
                          const std::vector<Pore> &pores,
                          const std::vector<Grain> &grains);


        template<typename Nodes, typename Pores, typename Grains, typename Fn, typename Fp, typename Fg>
        void print_net_ps_with_values(std::string title,
                                      Nodes &&nodes,
                                      Pores &&pores,
                                      Grains &&grains,
                                      Fn &&f_n,
                                      Fp &&f_p,
                                      Fg &&f_g);


        void save_VTU(const std::vector<Node> &nodes,
                      const std::vector<Pore> &pores,
                      const std::vector<Grain> &grains,
                      const bool add_grain_info = true);


        template<typename Elements, typename Func>
        void print_txt(OutStream &stream, std::string title, Elements &&elements, Func &&func) {
            log.log<LogLevel::DEBUG>("Printing txt...");
            stream << std::format("#Page {}: {}\n", stream.get_page_count(), title);
            for (const auto &el: elements)
                stream << std::format("{:4.5f}\t", double(func(el)));
            stream.add_page();
        }

        void write_csv_data();



        const PrintingConfig &config;
        PrintingModuleState state{  .print_txt_now = false,
                                    .print_ps_now  = false,
                                    .print_vtk_now = false};

    protected:

        Logger<logger_level_min> &log;
        const NetworkTopologyConfig &net_topo_config;
        const NetworkConfig &net_config;


        // output files
        std::ofstream time_evolution_out;            //always printed, basic information about each simulation step

        OutStream net_pores_ps{"net_pores.ps"}, net_grains_ps{"net_pores.ps"}, net_debug_ps{"net_debug.ps"};
        OutStream net_out{"net.vtu"}, pore_nodes_out{"pores_nodes.vtu"};            // VTK files
        OutStream d_out{"d.txt"}, q_out{"q.txt"}, u_out{"u.txt"}, l_out{"l.txt"};   // pure data files                                                             // Text data filse

        EnumArray<SOLUBLES, std::unique_ptr<OutStream>, enum_size_v<SOLUBLES>> C_out{
            [](SOLUBLES e){
                return std::make_unique<OutStream>("c_" + e + ".txt");}
        };

        EnumArray<SOLIDS, std::unique_ptr<OutStream>, enum_size_v<SOLIDS>> V_out{
                [](SOLIDS e){
                    return std::make_unique<OutStream>("v_" + e +".txt");}
        };



        // inlet files      //TODO: Inlet files to be implemented.
        std::ifstream 	conf_in, net_in, net_g_in, pores_in, grains_in;


    };
}


#endif //KARST_4_0_PRINTINGMODULE_H
