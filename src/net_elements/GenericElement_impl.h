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



}

#endif //KARST_4_0_GENERICELEMENT_IMPL_H
