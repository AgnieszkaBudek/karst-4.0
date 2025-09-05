//
// Created by Agnieszka on 24/07/2025.
//

#ifndef KARST_4_0_GENERICELEMENT_IMPL_H
#define KARST_4_0_GENERICELEMENT_IMPL_H


#include <algorithm>
#include <ranges>

#include "Node.h"
#include "Pore.h"
#include "Grain.h"


namespace karst {


    inline void Node::do_disconnect_from_network() {
        for (auto n: nodes)
            erase_if(n->nodes, [this](auto x) { return x == this; });
        for (auto p : pores)
            erase_if(p->nodes, [this](auto x) { return x == this; });
        for (auto g: grains)
            erase_if(g->nodes, [this](auto x) { return x == this; });
    }

    inline void Pore::do_disconnect_from_network() {
        for (auto n: nodes) {
            for( auto [n0,p0] : n->nodePores)
                if(p0 == this)
                    erase_if(n->nodes, [n0](auto x) { return x == n0; });

            erase_if(n->nodePores, [this](auto x) { return x.p == this; });
            erase_if(n->pores,     [this](auto x) { return x == this;   });

        }
        for (auto p : pores)
            erase_if(p->pores, [this](auto x) { return x == this; });
        for (auto g: grains)
            erase_if(g->pores, [this](auto x) { return x == this; });
    }


    inline void Grain::do_disconnect_from_network() {
        for (auto n: nodes)
            erase_if(n->grains, [this](auto x) { return x == this; });
        for (auto p : pores)
            erase_if(p->grains, [this](auto x) { return x == this; });
        for (auto g: grains)
            erase_if(g->grains, [this](auto x) { return x == this; });
    }


    auto Pore::get_available_volume(SOLIDS species) -> Volume{
        Volume sum = 0._V;
        for(auto g : grains)
            sum = sum + g->get_v(species);
        return sum;
    }

    auto Pore::get_surface   (SOLIDS sp) const -> Area {   //return surface in content with species sp
        int active_grains = 0;
        for(auto g : grains) if(g->if_species_available(sp)) active_grains++;
        double species_factor = active_grains/double(grains.size());
        return species_factor*get_surface_tot();
    }

    auto Pore::update_length() -> void{
        Unitless l_tot_f = 0._U;
        for(auto g : grains)
            l_tot_f +=  1._U*pow(double(g->get_tot_v()/g->get_max_volume()),net_config.l_V_scaling_f);

        auto l_tmp = get_l_max()*l_tot_f/double(get_grains().size());
        if(l_tmp>net_config.l_min)  set_l(l_tmp);
        else                        set_l(net_config.l_min);       //pore length cannot be too small
    }

    template < typename Element, typename ElementState>
    bool GenericElement<Element, ElementState> :: check_if_node_connected(Node* n){
        auto it = std::find_if(nodes.begin(), nodes.end(), [n](Node* e) {
            return e == n;
        });
        return it != nodes.end();
    }

    template < typename Element, typename ElementState>
    bool GenericElement<Element, ElementState> :: check_if_pore_connected(Pore* p){
        auto it = std::find_if(pores.begin(), pores.end(), [p](Pore* e) {
            return e == p;
        });
        return it != pores.end();
    }

    template < typename Element, typename ElementState>
    bool GenericElement<Element, ElementState> :: check_if_grain_connected(Grain* g){
        auto it = std::find_if(grains.begin(), grains.end(), [g](Grain* e) {
            return e == g;
        });
        return it != grains.end();
    }

}



#endif //KARST_4_0_GENERICELEMENT_IMPL_H
