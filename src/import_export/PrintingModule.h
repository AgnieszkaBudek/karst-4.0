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

        explicit PrintingModule(const NetworkTopologyConfig & net_conf0, const PrintingConfig& config0): net_topo_config{net_conf0}, config{config0}
        {
            if (config.do_save_table){
                d_out .open("d.out", std::ios_base::out | std::ios_base::trunc );
                q_out .open("q.out", std::ios_base::out | std::ios_base::trunc );
                u_out .open("u.out", std::ios_base::out | std::ios_base::trunc );
                c_out .open("c.out", std::ios_base::out | std::ios_base::trunc );
                V_out .open("V.out", std::ios_base::out | std::ios_base::trunc );
                l_out .open("l.out", std::ios_base::out | std::ios_base::trunc );
            }

            if (config.do_save_ps){
                net_pores_ps  .open("net_pores.ps", std::ios_base::out | std::ios_base::trunc );
                net_grains_ps .open("net_pores.ps", std::ios_base::out | std::ios_base::trunc );
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
            if (config.do_save_ps) {
                net_pores_ps    .close();
                net_grains_ps   .close();
            }
        }

        auto print_net_ps(
                const std::vector<Node>  &n,
                const std::vector<Pore>  &p,
                const std::vector<Grain> &g) -> void {
            do_print_net_ps( net_pores_ps,n,p,g); }

        const PrintingConfig& config;

    protected:

        const NetworkTopologyConfig & net_topo_config;

        // output files
        ofstream_ps_pores 	net_pores_ps;
        ofstream_ps_grains 	net_grains_ps;
        ofstream_txt    pores_out, nodes_out, grains_out, net_out, net_g_out, tmp_out;
        ofstream_txt    time_evolution_out, pattern_analysis_out, child_distribution_out, fork_distribution_out, cluster_size_out, child_distribution2_out, fork_distribution2_out, cluster_size2_out;
        ofstream_txt    d_out, q_out, u_out, c_out, V_out, l_out;

        // inlet files
        std::ifstream 	conf_in, net_in, net_g_in, pores_in, grains_in;
        std::ostream * log_out = &std::cerr;



        auto do_print_net_ps(ofstream_ps &stream,
                             const std::vector<Node>  &nodes,
                             const std::vector<Pore>  &pores,
                             const std::vector<Grain> &grains) -> void {

            std::cerr << "Printing postscript..." << std::endl;

            do_print_ps_headlines(stream,1,"Debugging...");

            for (const auto& g_tmp : grains)
                if(g_tmp.check_if_active())
                    stream << Polygon3D{.n = g_tmp.get_nodes(),.name = double(g_tmp.config.name)};

            for (const auto& p_tmp : pores)
                if(p_tmp.check_if_active())
                    stream << Pore3D{.p = p_tmp,.name = double(p_tmp.config.name)};

            for (const auto& n_tmp : nodes)
                if(n_tmp.check_if_active())
                    stream << Node3D{.n = n_tmp,.name = double(n_tmp.config.name)};

        }

        auto do_print_ps_headlines (std::ostream& stream, int page_nr, const std::string& description_note) -> void{

            int N = (net_topo_config.N_x);
            int M = (net_topo_config.N_y);
            double scale_tmp = 400. / (double(config.L_out) * std::max(N, M));
            double x_zero = 100. / scale_tmp;
            double y_zero = 750. / scale_tmp;

            if(page_nr==1)
                stream << "%!PS-Adobe-3.0"<<std::endl;

            stream << "%%Page: " << page_nr << " " << page_nr << std::endl << std::endl;
            stream << "%!PS-Adobe-3.0" << std::endl;
            stream << "1 setlinejoin 1 setlinecap 0.02 setlinewidth" << std::endl;

            stream << scale_tmp << "\t" << scale_tmp << " scale" << std::endl;
            stream << x_zero << "\t" << y_zero << " translate"<<std::endl;
            stream << "1 setlinejoin" << std::endl;
            stream << "1 setlinecap" << std::endl;

            //title
            stream << Color(0,0,0);
            stream << "/Times-Bold findfont " << 20. / scale_tmp << " scalefont setfont" << std::endl;
            stream << "0 " << -450. / scale_tmp << " moveto" << std::endl;
            stream << "0 0 ("<<description_note<<") ashow"<<std::endl;
            stream << " stroke" << std::endl << std::endl;
        }
    };
}


#endif //KARST_4_0_PRINTINGMODULE_H
