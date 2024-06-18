//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_PRINTINGMODULE_H
#define KARST_4_0_PRINTINGMODULE_H


#include "src/utils.h"
#include "src/units.h"
#include "src/network/NetworkTopologyConfig.h"
#include "src/network/NetworkConfig.h"
#include "PrintingConfig.h"


namespace karst {


    class PrintingModule {

    public:

        explicit PrintingModule(const PrintingConfig& config0): config(config0)
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

    protected:
        
        const PrintingConfig config;

        // output files
        ofstream_ps_pores 	net_pores_ps;
        ofstream_ps_grains 	net_grains_ps;
        ofstream_txt    pores_out, nodes_out, grains_out, net_out, net_g_out, tmp_out;
        ofstream_txt    time_evolution_out, pattern_analysis_out, child_distribution_out, fork_distribution_out, cluster_size_out, child_distribution2_out, fork_distribution2_out, cluster_size2_out;
        ofstream_txt    d_out, q_out, u_out, c_out, V_out, l_out;

        // inlet files
        std::ifstream 	conf_in, net_in, net_g_in, pores_in, grains_in;
        std::ostream * log_out = &std::cerr;

    };
}


#endif //KARST_4_0_PRINTINGMODULE_H
