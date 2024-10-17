//
// Created by Agnieszka on 23/05/2024.
//

#ifndef KARST_4_0_NETWORK_H
#define KARST_4_0_NETWORK_H

#include <utility>

#include "src/utils.h"
#include "src/units.h"
#include "src/net_elements/Node.h"
#include "src/net_elements/Pore.h"
#include "src/net_elements/Grain.h"
#include "src/import_export/PrintingModule.h"
#include "src/import_export/PrintingConfig.h"
#include "src/network/NetworkTopologyConfig.h"
#include "src/network/NetworkConfig.h"
#include "src/network/NetworkState.h"



namespace karst {

    void createHexagonalNetwork(Network& S, Int N, Int M);

    class Network {

    public:

        friend void createHexagonalNetwork(Network&,Int,Int);

        explicit Network (const NetworkConfig&  conf, const NetworkTopologyConfig&  t_conf, const PrintingConfig& p_conf):
        config{conf},t_config{t_conf}, io_mod{t_conf, p_conf}
        {}


        ~Network() {

            std::cerr << "Deleting network..." << std::endl;
        }

        auto save_network_state() ->void { do_save_network_state();}
        auto get_nodes()  const -> const std::deque<Node>&   {  return nodes; }
        auto get_pores()  const -> const std::deque<Pore>&   {  return pores; }
        auto get_grains() const -> const std::deque<Grain>&  {  return grains; }

        auto init() -> void {

            std::cerr<<"Initializing network..."<<std::endl;

            // 1. create geometry
            switch (t_config.type_of_topology) {
                case TypeOfNetTopology::HEXAGONAL:
                    createHexagonalNetwork(*this,t_config.N_x,t_config.N_y);
                    break;
                case TypeOfNetTopology::FROM_FILE:
                    std::cerr << "TypeOfNetTopology::FROM_FILE Not implemented yet!" << std::endl;
                    break;
                case TypeOfNetTopology::FROM_TRIANGULATION:
                    std::cerr << "TypeOfNetTopology::FROM_TRIANGULATION Not implemented yet." << std::endl;
                    break;
                default:
                    std::cerr << "Unknown geometry." << std::endl;
                    break;
            }

            //        check_network_connections();  //TODO: add a function checking network connections...

            // 2. Set initial element state

            for (auto& p : pores)   p.init();
            for (auto& g : grains)  g.init();
            for (auto& n : nodes)   n.init();


            // 3. Add boundary conditions
            if (!t_config.do_periodic_bc)
                for (auto& p : pores)
                    if (p.nodes.back()->get_pos() - p.nodes.front()->get_pos() > 5._U * config.l0)
                        p.state.d = Length(0);


            // 4. Delete unused elements

            if (t_config.do_clear_unused_pores) {
                std::cerr << "Clearing unused pores:" << std::endl;
                // clear_unused_pores();  //TODO: implement it
                std::cerr << "Clearing unused nodes:" << std::endl;
                // clear_unused_nodes();    //TODO: implement it
                std::cerr << "Clearing unused grains:" << std::endl;
                // clear_unused_grains();   //TODO: implement it
                std::cerr << std::endl;
                // check_network_connections(); //TODO: implement it
            }

        }

        const NetworkTopologyConfig&  t_config;
        const NetworkConfig&            config;

    protected:

        NetworkState      state;

        std::deque<Node>  nodes;
        std::deque<Pore>  pores;
        std::deque<Grain> grains;

        std::deque<Node*> n_inlet;
        std::deque<Node*> n_outlet;

        PrintingModule io_mod;


        auto do_save_network_state() -> void   //TODO: zastanowić się czy nie może to być jednak funkcja const?
        {
            std::cerr<<"Saving network state..."<<std::endl;
            std::cerr<<"Saving print_net_ps..."<<std::endl;
            if(io_mod.config.do_save_ps)
                io_mod.print_net_ps(nodes, pores, grains);
        }

    };

}

#endif //KARST_4_0_NETWORK_H
