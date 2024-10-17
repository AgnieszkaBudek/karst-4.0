//
// Created by Agnieszka on 23/05/2024.
//

#ifndef KARST_4_0_NODE_H
#define KARST_4_0_NODE_H

#include "src/utils.h"

#include "src/units.h"
#include "src/net_elements/GenericElement.h"
#include "src/import_export/printing_primitives.h"



namespace karst {

    class Point3D;

    enum class NodeType {
        INPUT,
        OUTPUT,
        NORMAL,
        NOT_CONNECTED
    };
    // Specialization of EnumToString
    template<>
    const std::map<NodeType, std::string> EnumToString<NodeType>::mapping = {
            { NodeType::NORMAL,         "NORMAL"        },
            { NodeType::NOT_CONNECTED,  "NOT_CONNECTED" },
            { NodeType::INPUT,          "INPUT"         },
            { NodeType::OUTPUT,         "OUTPUT"        }
    };
//    // Operator << specialization for NodeType   //TODO: wyrzucić potem
//    std::ostream& operator<<(std::ostream& os, NodeType value) {
//        return os << EnumToString<NodeType>::mapping.at(value);
//    }


    struct NodeState {

        Pressure u{};
        std::map <SPECIES, Concentration> c;              ///< concentration of specific species
    };


    class Node : public GenericElement <Node, NodeState> {

    public:

        explicit Node  (const NetworkConfig& net_conf0, const NetworkTopologyConfig &topo_conf0, const ElementConfig config0) : GenericElement<Node, NodeState>(net_conf0,topo_conf0,config0){}

        friend  GenericElement <Node, NodeState>;


        inline auto set_type (NodeType t0) -> void      { type = t0;   }

        inline auto get_type () const      -> NodeType  { return type; }

        inline auto set_pos (Point3D p0)   -> void      { pos = p0;    }

        inline auto get_pos () const       -> Point3D   { return pos;  }

        inline auto set_u (Pressure u0)    -> void      { state.u = u0;    }

        inline auto get_u () const         -> Pressure  { return state.u;  }

        inline auto set_c (SPECIES sp, Concentration c)  -> void { state.c[sp] = c;}

        inline auto get_c (SPECIES sp) const             -> Concentration{
            assert(state.c.find(sp) != state.c.end() && "Key not found in the map");
            return state.c.at(sp);
        }


        auto init() -> void
        {
            for (auto sp : solubleS)
                if (type == NodeType::INPUT){
                    assert(net_config.inlet_c.find(sp) != net_config.inlet_c.end() && "Key not found in the map");
                    state.c[sp] = net_config.inlet_c.at(sp);
                }
                else
                    state.c[sp] = Concentration{0.0};
        }



        //export functions

        friend auto operator<<(log_stream& os, const Node& obj) -> log_stream&
        {
            os <<  obj.config.type << ": "<< obj.config.name << std::endl;
            os <<"\tConcentration: ";
            for(auto&[key,value]: obj.state.c)
                os << key <<" <-> "<<value<<"\t";
            os << std::endl;
            return os;
        }

        //friend ofstream_ps_pores &operator <<(ofstream_ps_pores &stream, const Node &n){}     //TODO: implement it
        //friend ofstream_ps_grains &operator<<(ofstream_ps_grains &stream, const Node &n){}    //TODO: implement it



    protected:

        NodeType type {NodeType::NORMAL};
        Point3D  pos  {Length(NaN),Length(NaN),Length(NaN)};


    };
} // namespace karst

#endif //KARST_4_0_NODE_H
