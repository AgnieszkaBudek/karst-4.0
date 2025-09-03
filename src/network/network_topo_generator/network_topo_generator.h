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
            case TypeOfNetTopology::FROM_H_FILE:
                read_csv_H_data(*this);
                break;
            case TypeOfNetTopology::FROM_FILE:
                log.log<LogLevel::ERROR>("TypeOfNetTopology::FROM_FILE Not implemented yet!");
                break;
            case TypeOfNetTopology::FROM_TRIANGULATION:
                log.log<LogLevel::ERROR>("TypeOfNetTopology::FROM_TRIANGULATION Not implemented yet.");
                break;
            default:
                log.log<LogLevel::ERROR>( "Unknown geometry.");
                break;
        }

        // 2. Check connections:
        check_network_connections();  //TODO: add a function checking network connections...

//        for(auto& p : pores)
//            p.state.d=0.1_L;

        // 3. Disconnect inlet/outlet nodes
        for(auto& p : pores)
            if(p.nodes[0]->get_type() != NodeType::NORMAL and p.nodes[1]->get_type() != NodeType::NORMAL)
                p.deactivate();

//
//        for(auto& p : pores)
//            p.state.d=0.1_L;

        // 4. Cut boundary if not periodic bc
        if (!t_config.do_periodic_bc) {
            for (auto &p: pores)
                if (p.nodes.back()->get_pos() - p.nodes.front()->get_pos() > 10._U * config.l0)
                    p.deactivate();
        }


        // 5. Delete unused elements
        if (t_config.do_clear_unused_net_el) {
            log.log("Clearing unused elements of the network:");
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

        log.log( "Checking connections in the network...");

        apply_to_all_net_elements([&](auto& el) {
            if (!el.active)
                log.log_with_context<LogLevel::ERROR>( el ,"Nonactive element in the network.");
            for (auto p : el.pores)
                if(!p->active)
                    log.log_with_context<LogLevel::ERROR>(el,"Nonactive connection: with"+ p->get_context_info());
            for (auto n : el.nodes)
                if(!n->active)
                    log.log_with_context<LogLevel::ERROR>(el,"Nonactive connection: with"+ n->get_context_info());
            for (auto g : el.grains)
                if(!g->active)
                    log.log_with_context<LogLevel::ERROR>(el,"Nonactive connection: with"+ g->get_context_info());

        });

        log.log("Checking networks' topology...");
    }




}




#endif //KARST_4_0_NETWORK_TOPO_GENERATOR_H
