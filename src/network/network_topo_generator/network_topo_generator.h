//
// Created by Agnieszka on 25/07/2025.
//

#ifndef KARST_4_0_NETWORK_TOPO_GENERATOR_H
#define KARST_4_0_NETWORK_TOPO_GENERATOR_H

#include"src/network/Network.h"

namespace karst{



    auto Network::prepare_network_topology() -> void{


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

        // 2. Check connections:
        check_network_connections();  //TODO: add a function checking network connections...


        // 3. Disconnect inlet/outlet nodes
        for(auto& p : pores)
            if(p.nodes[0]->get_type() != NodeType::NORMAL and p.nodes[1]->get_type() != NodeType::NORMAL)
                p.deactivate();


        // 4. Add boundary conditions
        if (!t_config.do_periodic_bc) {
            for (auto &p: pores)
                if (p.nodes.back()->get_pos() - p.nodes.front()->get_pos() > 10._U * config.l0)
                    p.deactivate();
        }


        // 5. Delete unused elements
        if (t_config.do_clear_unused_net_el) {
            std::cerr << "Clearing unused elements of the network:" << std::endl;
            clear_unused_elements();
        }

        // 6. Check connections after deleting unused ones:
        check_network_connections();


    }


    auto Network::clear_unused_elements() -> void{

        set_percolation_cluster([](const Node&, const Pore&){return true;}, true);

        apply_to_all_net_elements([](auto& el) {
            if (!el.topo_s.connected_to_percolation_cluster)
                el.deactivate();
        });

        erase_if(n_inlet,  [](auto x) { return !x->active;});
        erase_if(n_outlet, [](auto x) { return !x->active;});
    }

    auto Network::check_network_connections() -> void{ //FIXME: finish implementing this one

        std::cerr<<"Checking connections in the network..."<<std::endl;

        apply_to_all_net_elements([](auto& el) {
            if (!el.active)
                std::cerr<<"Nonactive "<< el.config.type <<" nr"<<el.config.name<<" in the network."<<std::endl;
            for (auto p : el.pores)
                if(!p->active)
                    std::cerr<<"Nonactive connection: "<< el.config.type << " nr "<<el.config.name<<" <-> "
                    << p->config.type << " nr "<<p->config.name<<std::endl;
            for (auto n : el.nodes)
                if(!n->active)
                    std::cerr<<"Nonactive connection: "<< el.config.type << " nr "<<el.config.name<<" <-> "
                             << n->config.type << " nr "<<n->config.name<<std::endl;
            for (auto g : el.grains)
                if(!g->active)
                    std::cerr<<"Nonactive connection: "<< el.config.type << " nr "<<el.config.name<<" <-> "
                             << g->config.type << " nr "<<g->config.name<<std::endl;

        });

        std::cerr<<"Checking networks' topology..."<<std::endl;
    }




}




#endif //KARST_4_0_NETWORK_TOPO_GENERATOR_H
