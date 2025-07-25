//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_CREATE_HEXAGONAL_NETWORK_H
#define KARST_4_0_CREATE_HEXAGONAL_NETWORK_H

#include <random>
#include "src/utils.h"
#include "src/units.h"
#include "src/network/Network.h"


namespace karst {

    inline void createHexagonalNetwork(Network& S, Int N_x, Int N_y) {

        std::cerr << "Creating hexagonal network..." << std::endl;


        if (N_y % 2 == 1) {
            std::cerr << "ERROR: N_y must be even in hexagonal network!" << std::endl;
        }


        S.nodes.reserve(N_x*N_y);
        S.pores.reserve(N_x*N_y*3);
        S.grains.reserve(N_x*N_y*2);


// Creating all network elements: nodes, pores and grains

        for (auto i=0; i < N_x * N_y; i++)
            S.nodes.emplace_back(S.config, S.t_config, ElementConfig{.type=ElementType::NODE,.name=i,.log = &std::cerr});

        for (auto i=0; i < N_x * N_y * 3; i++)
            S.pores.emplace_back(S.config, S.t_config, ElementConfig{.type=ElementType::PORE,.name=i,.log = &std::cerr});

        for (auto i=0; i < N_x * N_y * 2; i++)
            S.grains.emplace_back(S.config, S.t_config, ElementConfig{.type=ElementType::GRAIN,.name=i,.log = &std::cerr});


        //pore and node connections
        for (auto j = 0; j < N_y; j++)
            for (auto i = 0; i < N_x; i++)
            {
                auto& n_tmp = S.nodes[N_x * j + i];

               n_tmp.set_nodes({
                                                 &S.nodes[N_x*((j+1)%N_y)     + (i-j%2+N_x)%N_x],
                                                 &S.nodes[N_x*((j+1)%N_y)     + (i-j%2+1)%N_x],
                                                 &S.nodes[N_x*j               + (i+1)%N_x],
                                                 &S.nodes[N_x*((j-1+N_y)%N_y) + (i-j%2+1)%N_x],
                                                 &S.nodes[N_x*((j-1+N_y)%N_y) + (i-j%2+N_x)%N_x],
                                                 &S.nodes[N_x*j               + (i-1+N_x)%N_x]
                                         });

                n_tmp.set_pores({
                                                 &S.pores[(N_x * j + i) * 3 + 0],
                                                 &S.pores[(N_x * j + i) * 3 + 1],
                                                 &S.pores[(N_x * j + i) * 3 + 2],
                                                 &S.pores[(N_x * ((j - 1 + N_y) % N_y) + (i - j % 2 + 1) % N_x) * 3 + 0],
                                                 &S.pores[(N_x * ((j - 1 + N_y) % N_y) + (i - j % 2 + N_x) % N_x) * 3 + 1],
                                                 &S.pores[(N_x * j + (i - 1 + N_x) % N_x) * 3 + 2]
                                         });

                n_tmp.set_node_pores();

                S.pores[(N_x * j + i) * 3 + 0].set_nodes({&n_tmp, n_tmp.nodes[0] });
                S.pores[(N_x * j + i) * 3 + 1].set_nodes({&n_tmp, n_tmp.nodes[1] });
                S.pores[(N_x * j + i) * 3 + 2].set_nodes({&n_tmp, n_tmp.nodes[2] });

                S.grains[(N_x * j + i) * 2 + 0].set_nodes({&n_tmp, n_tmp.nodes[0], n_tmp.nodes[1] });
                S.grains[(N_x * j + i) * 2 + 1].set_nodes({&n_tmp, n_tmp.nodes[1], n_tmp.nodes[2] });

            }

        //adding info grain -> pores
        for (auto j = 0; j < N_y; j++)
            for (auto i = 0; i < N_x; i++) {
                auto& n_tmp = S.nodes[N_x * j + i];
                S.grains[(N_x * j + i) * 2 + 0].set_pores(
                        {S.find_pore(&n_tmp, n_tmp.nodes[0]),
                         S.find_pore(&n_tmp, n_tmp.nodes[1]),
                         S.find_pore(n_tmp.nodes[0], n_tmp.nodes[1])});
                S.grains[(N_x * j + i) * 2 + 1].set_pores(
                        {S.find_pore(&n_tmp, n_tmp.nodes[1]),
                         S.find_pore(&n_tmp, n_tmp.nodes[2]),
                         S.find_pore(n_tmp.nodes[1], n_tmp.nodes[2])});
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
        for (auto i = 0; i < N_x; i++)   // FIXME: rewrite this loop to be more elegant (?) Maybe not necessary
            for (auto j = 0; j < N_y; j++) {
                S.nodes[N_x * j + i].set_pos (double(S.config.l0) * Point3D(
                        {.x=Length(i - j % 2 * 0.5), .y=Length(j * sqrt(3) / 2),.z=0._L}));
            }


        //inlet/outlet nodes:
        std::cerr << "Setting inlets and outlets..." << std::endl;
        if (S.t_config.do_radial_geometry){
            auto& n_in = S.nodes[N_x*(N_y/2-1)+N_x/2-1];
            S.n_inlet.push_back(&n_in);

            auto R_min = S.config.l0*(std::min(N_x,N_y)/2.) -3._L;
            S.n_outlet.reserve(N_x*N_y/2);
            for(auto& n : S.nodes)
                if(n.get_pos() - n_in.get_pos() > R_min)
                    S.n_outlet.push_back(&n);
        }
        else{
            if(S.t_config.point_inlet)
                S.n_inlet.push_back(&S.nodes[N_x/2]);
            else{
                S.n_inlet.reserve(N_x);
                for (auto i = 0; i < N_x; i++)
                    S.n_inlet.push_back(&S.nodes[i]);
            }
            if(S.t_config.point_outlet)
                S.n_outlet.push_back(&S.nodes[N_x/2]);
            else{
                S.n_outlet.reserve(N_x);
                for (auto i = 0; i < N_x; i++)
                    S.n_outlet.push_back(&S.nodes[N_x * N_y - 1 - i]);
            }
        }
        for (auto n : S.n_inlet)   n->set_type(NodeType::INPUT);
        for (auto n : S.n_outlet)  n->set_type(NodeType::OUTPUT);



        //adding random shift
        if(S.t_config.do_randomness_in_regular_net){
            std::cerr<<"Adding randomness to the network..."<<std::endl;
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
        std::cerr << "Hexagonal network has been created." << std::endl;

    }

}


#endif //KARST_4_0_CREATE_HEXAGONAL_NETWORK_H
