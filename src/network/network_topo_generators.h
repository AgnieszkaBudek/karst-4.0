//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_NETWORK_TOPO_GENERATORS_H
#define KARST_4_0_NETWORK_TOPO_GENERATORS_H

#include <random>
#include "src/utils.h"
#include "src/units.h"
#include "src/network/Network.h"


namespace karst {

    inline void createHexagonalNetwork(Network& S, Int N, Int M) {

        if (M % 2 == 1) {
            std::cerr << "ERROR: N_y must be even in hexagonal network!" << std::endl;
        }

        std::cerr << "Creating hexagonal network..." << std::endl;



// Creating all network elements: nodes, pores and grains

        for (auto i=0; i<N*M; i++)
            S.nodes.emplace_back(S.config, S.t_config, ElementConfig{.type=ElementType::NODE,.name=i,.log = &std::cerr});

        for (auto i=0; i<N*M*3; i++)
            S.pores.emplace_back(S.config, S.t_config, ElementConfig{.type=ElementType::PORE,.name=i,.log = &std::cerr});

        for (auto i=0; i<N*M*2; i++)
            S.grains.emplace_back(S.config, S.t_config, ElementConfig{.type=ElementType::GRAIN,.name=i,.log = &std::cerr});


        //pore and node connections
        for (auto j = 0; j < M; j++)
            for (auto i = 0; i < N; i++)
            {
                auto& n_tmp = S.nodes[N * j + i];

               n_tmp.set_nodes({
                                                 &S.nodes[N * ((j + 1) % M) + (i - j % 2 + N) % N],
                                                 &S.nodes[N * ((j + 1) % M) + (i - j % 2 + 1) % N],
                                                 &S.nodes[N * j + (i + 1 + N) % N],
                                                 &S.nodes[N * ((j - 1 + M) % M) + (i - j % 2 + N) % N],
                                                 &S.nodes[N * ((j - 1 + M) % M) + (i - j % 2 + N) % N],
                                                 &S.nodes[N * j + (i - 1 + N) % N]
                                         });

                n_tmp.set_pores({
                                                 &S.pores[(N * j + i) * 3 + 0],
                                                 &S.pores[(N * j + i) * 3 + 1],
                                                 &S.pores[(N * j + i) * 3 + 2],
                                                 &S.pores[(N * ((j - 1 + M) % M) + (i - j % 2 + 1) % N) * 3 + 0],
                                                 &S.pores[(N * ((j - 1 + M) % M) + (i - j % 2 + N) % N) * 3 + 1],
                                                 &S.pores[(N * j + (i - 1 + N) % N) * 3 + 2]
                                         });

                S.pores[(N * j + i) * 3 + 0].set_nodes({&n_tmp, n_tmp.nodes[0] });
                S.pores[(N * j + i) * 3 + 1].set_nodes({&n_tmp, n_tmp.nodes[1] });
                S.pores[(N * j + i) * 3 + 2].set_nodes({&n_tmp, n_tmp.nodes[2] });

                S.grains[(N * j + i) * 2 + 0].set_nodes({&n_tmp, n_tmp.nodes[0], n_tmp.nodes[1] });
                S.grains[(N * j + i) * 2 + 1].set_nodes({&n_tmp, n_tmp.nodes[1], n_tmp.nodes[2] });

                S.grains[(N * j + i) * 2 + 0].set_pores({n_tmp.pores[0], n_tmp.pores[1] });
                S.grains[(N * j + i) * 2 + 1].set_pores({n_tmp.pores[1], n_tmp.pores[2] });


            }

        //inlet/outlet nodes:

        for (auto i = 0; i < N; i++) {
            S.n_inlet.push_back(&S.nodes[i]);
            S.nodes[i].set_type(NodeType::INPUT);
            S.n_outlet.push_back(&S.nodes[N * M - 1 - i]);
            S.nodes[N * M - 1 - i].set_type (NodeType::OUTPUT);
        }

        //adding rest of the topology info

        //adding node -> grain info
        for (auto& g_tmp : S.grains)
            for (auto n_tmp : g_tmp.nodes)
                n_tmp->grains.push_back(&g_tmp);


        //adding pore -> grain info
        for (auto& g_tmp : S.grains)
            for (auto p_tmp : g_tmp.pores)
                p_tmp->grains.push_back(&g_tmp);

        //adding pore -> pore info
        for (auto& p_tmp : S.pores)
            for (auto n_tmp : p_tmp.nodes)
                for (auto pp : n_tmp->pores)
                    if(pp!=&p_tmp)
                        p_tmp.pores.push_back(pp);

        //adding grain -> grain info
        for (auto& g_tmp : S.grains)
            for (auto n_tmp : g_tmp.nodes)
                for (auto gg : n_tmp->grains)
                    if(gg!=&g_tmp)
                        g_tmp.grains.push_back(gg);



        //adding node Positions:
        for (auto i = 0; i < N; i++)
            for (auto j = 0; j < M; j++) {
                S.nodes[N * j + i].set_pos (Unitless(double(S.config.l0)) * Point3D({.x=Length(i - j % 2 * 0.5), .y=Length(j * sqrt(3) / 2),.z=0._L}));
            }

//        //random distribution  //TODO: add randomness to the node positions
//        std::default_random_engine gen;
//        std::normal_distribution<double> N_dist(); //distribution of initial diameters


        std::cerr << "Hexagonal network has been created." << std::endl;

    }

}


#endif //KARST_4_0_NETWORK_TOPO_GENERATORS_H
