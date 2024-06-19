//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_PRINTINGMODULE_H
#define KARST_4_0_PRINTINGMODULE_H


#include "src/utils.h"
#include "src/units.h"
#include "src/network/NetworkTopologyConfig.h"
#include "src/network/NetworkConfig.h"
#include "src/network/Network.h"
#include "PrintingConfig.h"
#include "src/import_export/printing_primitives.h"


namespace karst {


    class PrintingModule {

    public:

        explicit PrintingModule(const Network& S0, const PrintingConfig& config0): S{S0}, config{config0}
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

        auto print_net_ps() -> void {
            do_print_net_ps( net_pores_ps); }

        const PrintingConfig& config;

    protected:

        const Network & S;

        // output files
        ofstream_ps_pores 	net_pores_ps;
        ofstream_ps_grains 	net_grains_ps;
        ofstream_txt    pores_out, nodes_out, grains_out, net_out, net_g_out, tmp_out;
        ofstream_txt    time_evolution_out, pattern_analysis_out, child_distribution_out, fork_distribution_out, cluster_size_out, child_distribution2_out, fork_distribution2_out, cluster_size2_out;
        ofstream_txt    d_out, q_out, u_out, c_out, V_out, l_out;

        // inlet files
        std::ifstream 	conf_in, net_in, net_g_in, pores_in, grains_in;
        std::ostream * log_out = &std::cerr;



        auto do_print_net_ps(std::ostream &stream) -> void {

            std::cerr << "Printing network..." << std::endl;

            //if(S.print_diss_factor) S.check_diss_pattern(S.print_diss_factor);

            int N = (S.t_config.N_x);
            int M = (S.t_config.N_y);
            double scale_tmp = 400. / (double(config.L_out) * std::max(N, M));
            double x_zero = 100. / scale_tmp;
            double y_zero = 750. / scale_tmp;


            //if(S.pages_saved==0) stream << "%!PS-Adobe-3.0" << endl<<"%%Pages:"<<S.pages_tot<<endl<<endl;

            //stream <<"%%Page: "<<S.pages_saved+1<<" "<<S.pages_saved+1<<endl<<endl;
            stream << "1 setlinejoin 1 setlinecap 0.02 setlinewidth" << std::endl;

            stream << scale_tmp << "\t" << scale_tmp << " scale" << std::endl;
            stream << x_zero << "\t" << y_zero << " translate"<<std::endl;
            stream << "1 setlinejoin" << std::endl;
            stream << "1 setlinecap" << std::endl;

            //title
            stream << Color(0,0,0);
            stream << "/Times-Bold findfont " << 20. / scale_tmp << " scalefont setfont" << std::endl;
            stream << "0 " << -450. / scale_tmp << " moveto" << std::endl;
            //stream << "0 0 ("<<S.description_note<<") ashow
            stream << " stroke" << std::endl << std::endl;

            for (const auto& n_tmp : S.get_nodes())
                stream << Node3D{.n = n_tmp};

            for (const auto& p_tmp : S.get_pores())
                stream << Pore3D{.p = p_tmp};

            for (const auto& p_tmp : S.get_pores())
                stream << Pore3D{.p = p_tmp};

        }



    };
}


#endif //KARST_4_0_PRINTINGMODULE_H
