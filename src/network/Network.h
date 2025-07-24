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

    void createHexagonalNetwork(Network& S, Int N_x, Int N_y);

    class Network {

    public:

        friend void createHexagonalNetwork(Network&,Int,Int);
        friend Node; friend Pore; friend Grain;
        explicit Network (const NetworkConfig&  conf, const NetworkTopologyConfig&  t_conf, const PrintingConfig& p_conf):
        config{conf},t_config{t_conf}, io_mod{t_conf, p_conf}
        {}


        ~Network() {

            std::cerr << "Deleting network..." << std::endl;
        }

        auto save_network_state() ->void { do_save_network_state();}
        auto get_nodes()  const -> const std::vector<Node>&   {  return nodes; }
        auto get_pores()  const -> const std::vector<Pore>&   {  return pores; }
        auto get_grains() const -> const std::vector<Grain>&  {  return grains; }
        auto find_pore(Node *n1, Node *n2)  const -> Pore* {
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
            // setting node pore connections
            for (auto& n : nodes) n.set_node_pores();
            //        check_network_connections();  //TODO: add a function checking network connections...


            // 2. Set initial element state
            for (auto& p : pores)   p.init();
            for (auto& g : grains)  g.init();
            for (auto& n : nodes)   n.init();
            //  alternative for 3 upper lines
            //  apply_to_all_net_elements([](auto& el){el.init();});


            // 3. Add boundary conditions
            if (!t_config.do_periodic_bc) {
                for (auto &p: pores)
                    if (p.nodes.back()->get_pos() - p.nodes.front()->get_pos() > 5._U * config.l0)
                        p.deactivate();
            }
            else{  //cutting direct connections between inlet and outlet
                for (auto n : n_inlet)
                    for (int i=0; i<n->nodes.size();i++)
                        if (n->nodes[i]->get_type()==NodeType::OUTPUT)
                            n->pores[i]->deactivate ();
            }

            // 4. Delete unused elements

            if (t_config.do_clean_unused_pores) {
                std::cerr << "Clearing unused elements of the network:" << std::endl;
                clear_unused_elements();  //TODO: finish implementing

            }

        }

        const NetworkTopologyConfig&  t_config;
        const NetworkConfig&            config;

    protected:

        NetworkState      state;

        std::vector<Node>  nodes;
        std::vector<Pore>  pores;
        std::vector<Grain> grains;


        std::vector<Node*> n_inlet;
        std::vector<Node*> n_outlet;

        PrintingModule io_mod;


        auto do_save_network_state() -> void   //TODO: zastanowić się czy nie może to być jednak funkcja const?
        {
            std::cerr<<"Saving network state..."<<std::endl;
            std::cerr<<"Saving print_net_ps..."<<std::endl;
            if(io_mod.config.do_save_ps)
                io_mod.print_net_ps(nodes, pores, grains);
        }


        auto clear_unused_elements() -> void{
            set_percolation_cluster([](const Node&, const Pore&){return true;}, true);
            apply_to_all_net_elements([](auto& el) {
                if (!el.topo_s.connected_to_percolation_cluster) el.deactivate();
            });
            erase_if(n_inlet,  [](auto x) { return !x->active;});
            erase_if(n_outlet, [](auto x) { return !x->active;});
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

            //additionally we can ser pores and nodes belonging to the cluster
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
