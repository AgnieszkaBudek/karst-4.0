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
#include "src/net_elements/GenericElement_impl.h"
#include "src/import_export/PrintingModule.h"
#include "src/import_export/PrintingConfig.h"
#include "src/network/NetworkTopologyConfig.h"
#include "src/network/NetworkConfig.h"
#include "src/network/NetworkState.h"



namespace karst {


    class Network {

    public:

        friend void createHexagonalNetwork(Network&,Int,Int);
        friend Node; friend Pore; friend Grain;


        explicit Network (const NetworkConfig&  conf,
                          const NetworkTopologyConfig&  t_conf,
                          const PrintingConfig& p_conf,
                          Logger<logger_level_min>& log0,
                          PrintingModule& io0):
                          config{conf}, t_config{t_conf}, io_mod{io0}, log(log0) {}


        ~Network() { log.log<LogLevel::INFO>("Deleting network...");}

        auto save_network_state() ->void { do_save_network_state();}

        auto get_nodes()      { return nodes    | std::views::filter([](const Node& n)  { return n.active; });}
        auto get_pores()      { return pores    | std::views::filter([](const Pore& p)  { return p.active; });}
        auto get_grains()     { return grains   | std::views::filter([](const Grain& g) { return g.active; });}
        auto get_inlets()     { return n_inlet;}
        auto get_outlets()    { return n_outlet;}

        auto get_state()   const -> const NetworkState&   { return state;}


        static auto find_pore(Node *n1, Node *n2) -> Pore* {
            for(auto& [n0, p0] : n1->nodePores)
                if(n0 == n2)  return p0;
            return nullptr;
        }

        template<typename Func>
        void apply_to_all_net_elements(Func&& func) {
            for (auto& n : nodes)  if(n.active) func(n);
            for (auto& p : pores)  if(p.active) func(p);
            for (auto& g : grains) if(g.active) func(g);
        }

        template<typename Func>
        void apply_to_all_nodes(Func&& func)  {for (auto& n : nodes)  if(n.active) func(n);}
        template<typename Func>
        void apply_to_all_pores(Func&& func)  {for (auto& p : pores)  if(p.active) func(p);}
        template<typename Func>
        void apply_to_all_grains(Func&& func) {for (auto& g : grains) if(g.active) func(g);}


        auto init() -> void { do_init(); }


        const NetworkTopologyConfig&  t_config;
        const NetworkConfig&            config;
        PrintingModule&                 io_mod;
        Logger<logger_level_min>&      log;

    protected:

        NetworkState       state;

        std::vector<Node>  nodes;
        std::vector<Pore>  pores;
        std::vector<Grain> grains;


        std::vector<Node*> n_inlet;
        std::vector<Node*> n_outlet;



        auto prepare_network_topology()      -> void;
        auto clear_unused_elements()         -> void;
        auto check_network_connections()     -> void;

        auto update_number_of_active_nodes() -> void{
            state.N_active = std::ranges::count_if(nodes, [](const Node& n) {
                return n.active;
            });
        }

        auto update_number_of_active_connections() -> void{

            Long total_size = 0;
            for (auto& n: get_nodes()){
                if(n.type == NodeType::NORMAL)
                    for(auto nn : n.get_nodes())
                        if(nn->type == NodeType::NORMAL)
                            total_size++;
            }

            state.N_active_connections = total_size;
        }

        auto update_active_names_of_elements() -> void{
            int i=0; int j=0;
            for(auto& n : nodes) {
                if (n.active and n.type == NodeType::NORMAL) n.a_name = i++;
                else n.a_name = --j;
            }
            i=0;
            for(auto& p : pores) if(p.active)
                p.a_name = i++;
            i=0;
            for(auto& g : grains) if(g.active)
                g.a_name = i++;
        }


        auto do_init() -> void {

            log.log<LogLevel::INFO>("Initializing network...");

            // 1. Prepare the topology
            prepare_network_topology();

            // 2.  Update number of active elements
            update_number_of_active_nodes();
            update_number_of_active_connections();
            update_active_names_of_elements();


            // 3. Set initial element state
            apply_to_all_net_elements([](auto& el){
                el.init();
            });

        }


        auto do_save_network_state() -> void   //TODO: zastanowić się czy nie może to być jednak funkcja const?
        {
            log.log<LogLevel::INFO>("Saving network state...");
            log.log<LogLevel::INFO>("Saving print_net_ps...");
            if(io_mod.config.do_save_ps)
                io_mod.print_net_ps(nodes, pores, grains);
        }



        template<typename Predicate>
        auto set_percolation_cluster(Predicate&& pred, bool set_pores_nodes=false) ->void{

            apply_to_all_net_elements([](auto& el) {el.topo_s = {false,false};});  // check if  CRTP works here

            std::function<void(Node&)> add_to_cluster = [&pred, &add_to_cluster](Node& nn){
                nn.topo_s.connected_to_percolation_cluster = true;
                if (nn.type==NodeType::OUTPUT) return;
                for( auto& [n0,p0] : nn.nodePores)
                    if(!n0->topo_s.checked_for_percolation){
                        n0->topo_s.checked_for_percolation=true;
                        if(pred(*n0, *p0))
                            add_to_cluster(*n0);
                    }
            };

            for (auto n : n_inlet) add_to_cluster(*n);

            //additionally we can set pores and nodes belonging to the cluster
            if (set_pores_nodes)
                for( auto& n : nodes) if(n.topo_s.connected_to_percolation_cluster)
                    for(auto& [n0,p0] : n.nodePores)
                        if(n0->topo_s.connected_to_percolation_cluster and n0->type!=NodeType::OUTPUT) {
                            Pore* p_tmp = find_pore(n0, &n);
                            p_tmp->topo_s.connected_to_percolation_cluster = true;
                            for(auto g_tmp : p_tmp->grains)
                                g_tmp->topo_s.connected_to_percolation_cluster = true;
                        }
        }

    };

}

#endif //KARST_4_0_NETWORK_H
