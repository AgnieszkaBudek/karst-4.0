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
            case TypeOfNetTopology::CUBIC:
                create_cubic_network(*this,t_config.N_x,t_config.N_y,t_config.N_z);
                break;
            case TypeOfNetTopology::FROM_H_FILE:
                read_csv_H_data(*this);
                break;
            case TypeOfNetTopology::FROM_FILE:
                log.log<LogLevel::ERROR>("TypeOfNetTopology::FROM_FILE Not implemented yet!");
                break;
            case TypeOfNetTopology::TRIANGULATION_2D:
                create_2D_triangulation(*this,t_config.N_x,t_config.N_y);
                break;
            default:
                log.log<LogLevel::ERROR>( "Unknown geometry.");
                break;
        }

        if constexpr (logger_level_min==LogLevel::DEBUG_PS)
            io_mod.print_net_ps_with_values(
                    "DEBUGGING in prepare_network_topology...",
                    get_nodes(),
                    get_pores(),
                    std::vector<Grain>{},
                    [](auto& el){return std::format("{:3.0f}",double(el.config.name));},
                    [](auto& el){return std::format("{:3.0f}",double(el.config.name));},
                    [](auto& el){return std::format("{:3.0f}",double(el.config.name));}
            );


        // 2. Check connections:
        check_network_connections();  //TODO: add a function checking network connections...


        // 3. Disconnect inlet/outlet nodes
        for(auto& p : pores)
            if(p.nodes[0]->get_type() != NodeType::NORMAL and p.nodes[1]->get_type() != NodeType::NORMAL)
                p.deactivate();

        if(!t_config.do_periodic_bc or !t_config.point_inlet or !t_config.point_outlet)
            for(auto& p : pores)        //disconnect
                    if(abs(p.nodes[0]->pos.y - p.nodes[1]->pos.y) > t_config.N_y*0.5_L){
                        log.log("DUPA22.");
                        p.deactivate();
                    }



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

            log.log(std::format("Number of active nodes:  {}",  state.N_active));
            log.log(std::format("Number of active pores:  {}",  state.P_active));
            log.log(std::format("Number of active grains: {}", state.G_active));

            io_mod.save_VTU(get_nodes(),get_pores(),get_grains(),false);

        }

        // 6. Check connections after deleting unused ones:
        check_network_connections();

        if constexpr (logger_level_min==LogLevel::DEBUG_PS)
            io_mod.print_net_ps_with_values(
                    "DEBUGGING after prepare_network_topology...",
                    get_nodes(),
                    get_pores(),
                    std::vector<Grain>{},
                    [](auto& el){return std::format("{:3.0f}",double(el.config.name));},
                    [](auto& el){return std::format("{:3.0f}",double(el.config.name));},
                    [](auto& el){return std::format("{:3.0f}",double(el.config.name));}
            );

    }


    auto Network::clear_unused_elements() -> void{

        set_percolation_cluster([](const Node&, const Pore&){return true;}, true);

        apply_to_all_net_elements([](auto& el) {
            if (!el.topo_s.connected_to_percolation_cluster)
                el.deactivate();
        });

        //FIXME: coś tu nie działa, bo zostajęz jednym niepodłączonym porem.
        for(auto &n : n_inlet)
            //n_outlet will be cleaned automatically
            if(n->pores.size()==0){
                log.log("DUPA11.");
                n->deactivate();
        }


        erase_if(n_inlet,  [](auto x) { return !x->active;});
        erase_if(n_outlet, [](auto x) { return !x->active;});

        update_number_of_active_nodes();
        update_number_of_active_pores();
        update_number_of_active_grains();
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

        for (auto& pore : pores)
            for (auto n : pore.nodes)
                if (! n->check_if_pore_connected(&pore))
                    log.log_with_context<LogLevel::ERROR>(*n,std::format("Problem with connection pore {}",pore.config.name));

        for (auto& pore : pores)
            for (auto p : pore.pores)
                if (! p->check_if_pore_connected(&pore))
                    log.log_with_context<LogLevel::ERROR>(*p,std::format("Problem with connection pore {}",pore.config.name));

        for (auto& pore : pores)
            for (auto g : pore.grains)
                if (! g->check_if_pore_connected(&pore))
                    log.log_with_context<LogLevel::ERROR>(*g,std::format("Problem with connection pore {}",pore.config.name));

        for (auto& node : nodes)
            for (auto n : node.nodes)
                if (! n->check_if_node_connected(&node))
                    log.log_with_context<LogLevel::ERROR>(*n,std::format("Problem with connection node {}",node.config.name));

        for (auto& node : nodes)
            for (auto p : node.pores)
                if (! p->check_if_node_connected(&node))
                    log.log_with_context<LogLevel::ERROR>(*p,std::format("Problem with connection node {}",node.config.name));

        for (auto& node : nodes)
            for (auto g : node.grains)
                if (! g->check_if_node_connected(&node))
                    log.log_with_context<LogLevel::ERROR>(*g,std::format("Problem with connection node {}",node.config.name));

        for (auto& grain : grains)
            for (auto n : grain.nodes)
                if (! n->check_if_grain_connected(&grain))
                    log.log_with_context<LogLevel::ERROR>(*n,std::format("Problem with connection grain {}",grain.config.name));

        for (auto& grain : grains)
            for (auto p : grain.pores)
                if (! p->check_if_grain_connected(&grain))
                    log.log_with_context<LogLevel::ERROR>(*p,std::format("Problem with connection grain {}",grain.config.name));

        for (auto& grain : grains)
            for (auto g : grain.grains)
                if (! g->check_if_grain_connected(&grain))
                    log.log_with_context<LogLevel::ERROR>(*g,std::format("Problem with connection grain {}",grain.config.name));

    }




}




#endif //KARST_4_0_NETWORK_TOPO_GENERATOR_H
