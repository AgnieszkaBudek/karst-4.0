//
// Created by Agnieszka on 24/07/2025.
//

#ifndef KARST_4_0_GENERICELEMENT_IMPL_H
#define KARST_4_0_GENERICELEMENT_IMPL_H


#include <algorithm>

#include "Node.h"
#include "Pore.h"
#include "Grain.h"


namespace karst {


    inline void Node::disconnect_from_network() {
        for (auto n: nodes)
            erase_if(n->nodes, [this](auto x) { return x == this; });
        for (auto p : pores)
            erase_if(p->nodes, [this](auto x) { return x == this; });
        for (auto g: grains)
            erase_if(g->nodes, [this](auto x) { return x == this; });
    }

    inline void Pore::disconnect_from_network() {
        for (auto n: nodes) {
            erase_if(n->pores,     [this](auto x) { return x == this;   });
            erase_if(n->nodePores, [this](auto x) { return x.p == this; });
        }
        for (auto p : pores)
            erase_if(p->pores, [this](auto x) { return x == this; });
        for (auto g: grains)
            erase_if(g->pores, [this](auto x) { return x == this; });
    }


    inline void Grain::disconnect_from_network() {
        for (auto n: nodes)
            erase_if(n->grains, [this](auto x) { return x == this; });
        for (auto p : pores)
            erase_if(p->grains, [this](auto x) { return x == this; });
        for (auto g: grains)
            erase_if(g->grains, [this](auto x) { return x == this; });
    }

}

#endif //KARST_4_0_GENERICELEMENT_IMPL_H
