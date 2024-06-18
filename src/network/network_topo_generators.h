//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_NETWORK_TOPO_GENERATORS_H
#define KARST_4_0_NETWORK_TOPO_GENERATORS_H

#include <random>
#include "src/utils.h"
#include "src/units.h"
#include "src/network/Network.h"
#include "src/net_elements/GenericElement.h"
#include "src/net_elements/Node.h"
#include "src/net_elements/Pore.h"
#include "src/net_elements/Grain.h"

namespace karst {

    void createHexagonalNetwork(Network& S, Int N, Int M) {

        if (M % 2 == 1) {
            std::cerr << "ERROR: N_y must be even in hexagonal network!" << std::endl;
        }

        std::cerr << "Creating hexagonal network..." << std::endl;



// Creating all network elements: nodes, pores and grains

        for (auto i=0; i<N*M; i++)
            S.n.emplace_back(S, ElementConfig{.type=NODE,.name=i,.log = &std::cerr});

        for (auto i=0; i<N*M*3; i++)
            S.p.emplace_back(S, ElementConfig{.type=PORE,.name=i,.log = &std::cerr});

        for (auto i=0; i<N*M*2; i++)
            S.g.emplace_back(S, ElementConfig{.type=PORE,.name=i,.log = &std::cerr});


        //pore and node connections
        for (auto j = 0; j < M; j++)
            for (auto i = 0; i < N; i++)
            {
                auto& n_tmp = S.n[N*j + i];

               n_tmp.set_nodes({
                                                 &S.n[N * ((j + 1) % M) + (i - j % 2 + N) % N],
                                                 &S.n[N * ((j + 1) % M) + (i - j % 2 + 1) % N],
                                                 &S.n[N * j + (i + 1 + N) % N],
                                                 &S.n[N * ((j - 1 + M) % M) + (i - j % 2 + N) % N],
                                                 &S.n[N * ((j - 1 + M) % M) + (i - j % 2 + N) % N],
                                                 &S.n[N * j + (i - 1 + N) % N]
                                         });

                n_tmp.set_pores({
                                                 &S.p[(N * j + i) * 3 + 0],
                                                 &S.p[(N * j + i) * 3 + 1],
                                                 &S.p[(N * j + i) * 3 + 2],
                                                 &S.p[(N * ((j - 1 + M) % M) + (i - j % 2 + 1) % N) * 3 + 0],
                                                 &S.p[(N * ((j - 1 + M) % M) + (i - j % 2 + N) % N) * 3 + 1],
                                                 &S.p[(N * j + (i - 1 + N) % N) * 3 + 2]
                                         });

                S.p[(N * j + i)*3 + 0].set_nodes({ &n_tmp, n_tmp.n[0] });
                S.p[(N * j + i)*3 + 1].set_nodes({ &n_tmp, n_tmp.n[1] });
                S.p[(N * j + i)*3 + 2].set_nodes({ &n_tmp, n_tmp.n[2] });

                S.g[(N * j + i) * 2 + 0].set_nodes({ &n_tmp, n_tmp.n[0], n_tmp.n[1] });
                S.g[(N * j + i) * 2 + 1].set_nodes({ &n_tmp, n_tmp.n[1], n_tmp.n[2] });

                S.g[(N * j + i) * 2 + 0].set_pores({ n_tmp.p[0], n_tmp.p[1] });
                S.g[(N * j + i) * 2 + 1].set_pores({ n_tmp.p[1], n_tmp.p[2] });


            }

        //inlet/outlet nodes:

        for (auto i = 0; i < N; i++) {
            S.n_inlet.push_back(&S.n[i]);
            S.n[i].set_type(Node::NodeType::INPUT);
            S.n_outlet.push_back(&S.n[N * M - 1 - i]);
            S.n[N * M - 1 - i].set_type (Node::NodeType::OUTPUT);
        }

        //adding rest of the topology info

        //adding node -> grain info
        for (auto& g_tmp : S.g)
            for (auto n_tmp : g_tmp.n)
                n_tmp->g.push_back(&g_tmp);


        //adding pore -> grain info
        for (auto& g_tmp : S.g)
            for (auto p_tmp : g_tmp.p)
                p_tmp->g.push_back(&g_tmp);

        //adding pore -> pore info
        for (auto& p_tmp : S.p)
            for (auto n_tmp : p_tmp.n)
                for (auto pp : n_tmp->p)
                    if(pp!=&p_tmp)
                        p_tmp.p.push_back(pp);

        //adding grain -> grain info
        for (auto& g_tmp : S.g)
            for (auto n_tmp : g_tmp.n)
                for (auto gg : n_tmp->g)
                    if(gg!=&g_tmp)
                        g_tmp.g.push_back(gg);



        //adding node Positions:
        for (auto i = 0; i < N; i++)
            for (auto j = 0; j < M; j++) {
                S.n[N * j + i].set_pos (Unitless(S.config.l0) * Point3D(Length(i - j % 2 * 0.5), Length(j * sqrt(3) / 2), 0._L));
            }

//        //random distribution  //TODO: add randomness to the node positions
//        std::default_random_engine gen;
//        std::normal_distribution<double> N_dist(); //distribution of initial diameters


        std::cerr << "Hexagonal network has been created." << std::endl;

    }

}


#endif //KARST_4_0_NETWORK_TOPO_GENERATORS_H
