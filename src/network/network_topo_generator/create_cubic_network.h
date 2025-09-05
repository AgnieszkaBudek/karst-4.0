//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_CREATE_CUBIC_NETWORK_H
#define KARST_4_0_CREATE_CUBICL_NETWORK_H

#include <random>
#include "src/utils.h"
#include "src/units.h"
#include "src/network/Network.h"


namespace karst {

    inline void create_cubic_network(Network& S, Int N_x, Int N_y, Int N_z) {

        S.log.log("Creating cubic network...");


        S.nodes.reserve(N_x*N_y*N_z);
        S.pores.reserve(N_x*N_y*3);
        S.grains.reserve(N_x*N_y*N_z);


// Creating all network elements: nodes, pores and grains

        for (auto i=0; i < N_x * N_y * N_z; i++)
            S.nodes.emplace_back(S.config, S.t_config, ElementConfig{.type=ElementType::NODE,.name=i,.log = S.log});

        for (auto i=0; i < N_x * N_y * N_z * 3; i++)
            S.pores.emplace_back(S.config, S.t_config, ElementConfig{.type=ElementType::PORE,.name=i,.log = S.log});

        for (auto i=0; i < N_x * N_y * N_z; i++)
            S.grains.emplace_back(S.config, S.t_config, ElementConfig{.type=ElementType::GRAIN,.name=i,.log = S.log});


        //pore and node connections
        for (auto k = 0; k < N_z; k++)
            for (auto j = 0; j < N_y; j++)
                for (auto i = 0; i < N_x; i++){
                    auto& n_tmp = S.nodes[N_y*N_x*k + N_x*j + i];
                    n_tmp.set_pos (double(S.config.l0) * Point3D({.x=Length(i), .y=Length(j),.z=Length(k)}));

                   n_tmp.set_nodes({
                                                     &S.nodes[N_y*N_x*k + N_x*j + (i+1)% N_x],
                                                     &S.nodes[N_y*N_x*k + N_x*j + (i-1+N_x) % N_x],
                                                     &S.nodes[N_y*N_x*k + N_x*((j+1)%N_y) + i],
                                                     &S.nodes[N_y*N_x*k + N_x*((j-1+N_y)%N_y) + i],
                                                     &S.nodes[N_y*N_x*((k+1)%N_z) + N_x*j + i],
                                                     &S.nodes[N_y*N_x*((k-1+N_z)%N_z) + N_x*j + i]
                                             });

                    n_tmp.set_pores({
                                                     &S.pores[(N_y*N_x*k + N_x*j + i)*3 + 0],
                                                     &S.pores[(N_y*N_x*k + N_x*j + (i-1+N_x)%N_x)*3 + 0],
                                                     &S.pores[(N_y*N_x*k + N_x*j + i)*3 + 1],
                                                     &S.pores[(N_y*N_x*k + N_x*((j-1+N_y)%N_y) + i)*3 + 1],
                                                     &S.pores[(N_y*N_x*k + N_x*j + i)*3 + 2],
                                                     &S.pores[(N_y*N_x*((k-1+N_z)%N_z) + N_x*j + i)*3 + 2]
                                             });

                    n_tmp.set_node_pores();

                    S.pores[(N_y*N_x*k + N_x*j + i)*3 + 0].set_nodes({&n_tmp, n_tmp.nodes[0] });
                    S.pores[(N_y*N_x*k + N_x*j + i)*3 + 1].set_nodes({&n_tmp, n_tmp.nodes[2] });
                    S.pores[(N_y*N_x*k + N_x*j + i)*3 + 2].set_nodes({&n_tmp, n_tmp.nodes[4] });
                    }

        //pore and node connections
        for (auto k = 0; k < N_z; k++)
            for (auto j = 0; j < N_y; j++)
                for (auto i = 0; i < N_x; i++){
                    auto& n_tmp = S.nodes[N_y*N_x*k + N_x*j + i];
                    Node * n1 = n_tmp.nodes[0]->nodes[2]->nodes[4];
                    S.grains[N_y*N_x*k + N_x*j + i].set_nodes({&n_tmp,
                                                               n_tmp.nodes[0],
                                                               n_tmp.nodes[2],
                                                               n_tmp.nodes[4],
                                                               n1,
                                                               n1->nodes[1],
                                                               n1->nodes[3],
                                                               n1->nodes[5]});
                    S.grains[N_y*N_x*k + N_x*j + i].set_pores({n_tmp.pores[0],
                                                               n_tmp.pores[2],
                                                               n_tmp.pores[4],
                                                               n1->pores[1],
                                                               n1->pores[3],
                                                               n1->pores[5],
                                                               n_tmp.nodes[0]->pores[2],
                                                               n_tmp.nodes[0]->pores[4],
                                                               n_tmp.nodes[2]->pores[4],
                                                               n_tmp.nodes[2]->pores[0],
                                                               n_tmp.nodes[4]->pores[2],
                                                               n_tmp.nodes[4]->pores[0]});

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


        //inlet/outlet nodes:
        S.log.log("Setting inlets and outlets...");
        if (S.t_config.do_radial_geometry){
            S.log.log<LogLevel::ERROR>("Radial geometry is not implemented for cubic network.");
        }
        else{
            if(S.t_config.point_inlet)
                S.n_inlet.push_back(&S.nodes[int(N_x*N_y/2+N_x/2-1)]);
            else{
                S.n_inlet.reserve(N_x*N_y);
                for (auto j = 0; j < N_y; j++)
                    for (auto i = 0; i < N_x; i++)
                    S.n_inlet.push_back(&S.nodes[N_x*j + i]);
            }
            if(S.t_config.point_outlet)
                S.n_outlet.push_back(&S.nodes[int(N_x*N_y*N_z-(N_x*N_y/2+N_x/2))]);
            else{
                S.n_outlet.reserve(N_x*N_y);
                for (auto j = 0; j < N_y; j++)
                    for (auto i = 0; i < N_x; i++)
                        S.n_outlet.push_back(&S.nodes[N_x * N_y*N_z - 1 - i - N_x*j]);
            }
        }
        for (auto n : S.n_inlet)   n->set_type(NodeType::INPUT);
        for (auto n : S.n_outlet)  n->set_type(NodeType::OUTPUT);



        //adding random shift for node position
        if(S.t_config.do_randomness_in_regular_net){
            S.log.log("Adding randomness to the network...");
            std::mt19937 gen(S.t_config.random_seed);
            std::uniform_real_distribution<double> dist(-1.0, 1.0);
            for (auto& n : S.nodes){
                    Point3D shift = Point3D{
                            S.t_config.max_rand_shift*S.config.l0*dist(gen),
                            S.t_config.max_rand_shift*S.config.l0*dist(gen),
                            0._L};
                    n.set_pos(n.get_pos()+shift);
                }
        }
        S.log.log("Cubic network has been created.");

    }

}


#endif //KARST_4_0_CREATE_CUBIC_NETWORK_H
