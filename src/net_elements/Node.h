//
// Created by Agnieszka on 23/05/2024.
//

#ifndef KARST_4_0_NODE_H
#define KARST_4_0_NODE_H

#include "src/utils.h"
#include "src/units.h"
#include "src/net_elements/GenericElement.h"




namespace karst {


    struct NodeState {

        std::map <SolubleChemical, Concentration> c;              ///< Volume of specific species

    };


    class Node : public GenericElement <Node, NodeState> {



    public:

        friend  GenericElement <Node, NodeState>;

        enum NodeType {
            INPUT,
            OUTPUT,
            NORMAL,
            NOT_CONNECTED
        };



    protected:

        NodeType type {NOT_CONNECTED};


    };
} // namespace karst

#endif //KARST_4_0_NODE_H
