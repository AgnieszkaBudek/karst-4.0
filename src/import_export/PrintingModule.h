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



namespace karst {



    class PrintingModule {

    public:

        explicit PrintingModule(const NetworkTopologyConfig & net_conf0,
                                const PrintingConfig& config0,
                                Logger<logger_level_min>& log0):
                                net_topo_config{net_conf0},
                                config{config0},
                                log{log0}
        {
            if (config.do_save_table){
                d_out .open("d.out", std::ios_base::out | std::ios_base::trunc );
                q_out .open("q.out", std::ios_base::out | std::ios_base::trunc );
                u_out .open("u.out", std::ios_base::out | std::ios_base::trunc );
                c_out .open("c.out", std::ios_base::out | std::ios_base::trunc );
                V_out .open("V.out", std::ios_base::out | std::ios_base::trunc );
                l_out .open("l.out", std::ios_base::out | std::ios_base::trunc );
            }

        };


        ~PrintingModule() {
            if (config.do_save_table){
                d_out .close();
                q_out .close();
                u_out .close();
                c_out .close();
                V_out .close();
                l_out .close();
            }
        }

        auto print_net_ps(const std::string& title,
                const std::vector<Node>  &n,
                const std::vector<Pore>  &p,
                const std::vector<Grain> &g) -> void {
            do_print_net_ps( net_pores_ps,title,n,p,g); }


        template<typename Nodes, typename Pores, typename Grains,typename Fn, typename Fp, typename Fg>
        void print_net_ps_with_values(std::string title,
                                      Nodes &&nodes,
                                      Pores &&pores,
                                      Grains &&grains,
                                      Fn &&f_n,
                                      Fp &&f_p,
                                      Fg &&f_g)
        {
            do_print_net_ps_with_values(net_debug_ps, title,
                                        std::forward<Nodes>(nodes),
                                        std::forward<Pores>(pores),
                                        std::forward<Grains>(grains),
                                        std::forward<Fn>(f_n),
                                        std::forward<Fp>(f_p),
                                        std::forward<Fg>(f_g));
        }


        const PrintingConfig& config;

    protected:

        Logger<logger_level_min>& log;
        const NetworkTopologyConfig & net_topo_config;


        // output files
        OutStream 	net_pores_ps  {"net_pores.ps"};
        OutStream 	net_grains_ps {"net_pores.ps"};
        OutStream   net_debug_ps  {"net_debug.ps"};
        std::ofstream    pores_out, nodes_out, grains_out, net_out, net_g_out, tmp_out;
        std::ofstream    time_evolution_out, pattern_analysis_out, child_distribution_out, fork_distribution_out, cluster_size_out, child_distribution2_out, fork_distribution2_out, cluster_size2_out;
        std::ofstream    d_out, q_out, u_out, c_out, V_out, l_out;

        // inlet files
        std::ifstream 	conf_in, net_in, net_g_in, pores_in, grains_in;





        auto do_print_net_ps(OutStream &stream, const std::string& title,
                             const std::vector<Node>  &nodes,
                             const std::vector<Pore>  &pores,
                             const std::vector<Grain> &grains) -> void {

            log.log<LogLevel::INFO>("Printing postscript...");

            stream.add_page();
            do_print_ps_headlines(stream,stream.get_page_count(),title);

            for (const auto& g_tmp : grains)
                if(g_tmp.check_if_active())
                    stream << Polygon3D{.n = g_tmp.get_nodes(),.name = std::to_string(g_tmp.config.name)};

            for (const auto& p_tmp : pores)
                if(p_tmp.check_if_active())
                    stream << Pore3D{.p = p_tmp,.name = std::to_string(p_tmp.config.name)};

            for (const auto& n_tmp : nodes)
                if(n_tmp.check_if_active())
                    stream << Node3D{.n = n_tmp,.name = std::to_string(n_tmp.config.name)};

            stream << "\nshowpage\n"<<fflush;

        }

        auto do_print_ps_headlines (OutStream& stream, int page_nr, const std::string& description_note) -> void{

            int N = (net_topo_config.N_x);
            int M = (net_topo_config.N_y);
            double scale_tmp = 400. / (double(config.L_out) * std::max(N, M));
            double x_zero = 100. / scale_tmp;
            double y_zero = 750. / scale_tmp;

            if(page_nr==1){
                stream << "%!PS-Adobe-3.0\n";
                stream << "%%Pages:100\n";
            }


            stream << "%%Page: " << page_nr << " " << page_nr <<"\n\n";
            stream << "1 setlinejoin 1 setlinecap 0.02 setlinewidth\n";

            stream << scale_tmp << "\t" << scale_tmp << " scale\n";
            stream << x_zero << "\t" << y_zero << " translate\n";
            stream << "1 setlinejoin\n";
            stream << "1 setlinecap\n";

            //title
            stream << Color(0,0,0);
            stream << "/Times-Bold findfont " << 20. / scale_tmp <<" scalefont setfont\n";
            stream << "0 " << -450. / scale_tmp << " moveto\n";
            stream << "0 0 ("<<description_note<<") ashow\n";
            stream << " stroke\n\n";
        }



        template<typename Nodes, typename Pores, typename Grains,typename Fn, typename Fp, typename Fg>
        void do_print_net_ps_with_values(OutStream &stream,
                                         std::string &title,
                                         Nodes &&nodes,
                                         Pores &&pores,
                                         Grains &&grains,
                                         Fn &&f_n,
                                         Fp &&f_p,
                                         Fg &&f_g)
        {
            log.log<LogLevel::INFO>("Printing postscript: debug for " + title + "...");

            stream.add_page();
            do_print_ps_headlines(stream,stream.get_page_count(),title);


            for (const auto &g_tmp : grains)
                if (g_tmp.check_if_active())
                    stream << Polygon3D{.n = g_tmp.get_nodes(),. name = std::forward<Fg>(f_g)(g_tmp)};

            for (const auto &p_tmp : pores)
                if (p_tmp.check_if_active())
                    stream << Pore3D{.p = p_tmp, .name = std::forward<Fp>(f_p)(p_tmp)};

            for (const auto &n_tmp : nodes)
                if (n_tmp.check_if_active())
                    stream << Node3D{.n = n_tmp, .name = std::forward<Fn>(f_n)(n_tmp)};

            stream << "\nshowpage\n"<<fflush;

        }



    };
}


#endif //KARST_4_0_PRINTINGMODULE_H
