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
        SIZE
    };
    // Specialization of EnumToString
    template<>
    const std::map<NodeType, std::string> EnumToString<NodeType>::mapping = {
            { NodeType::NORMAL,         "NORMAL"        },
            { NodeType::INPUT,          "INPUT"         },
            { NodeType::OUTPUT,         "OUTPUT"        }
    };



    struct NodeState {

        Pressure u{NaN};
        EnumArray <SOLUBLES, Concentration,enum_size_v<SOLUBLES>> c{0._C};     ///< concentration of specific species
    };

    struct NodePore{
        Node* n{};
        Pore* p{};
    };

    class Node : public GenericElement <Node, NodeState> {

    public:

        friend  GenericElement <Node, NodeState>;
        friend Network; friend Pore; friend Grain;
        friend auto createHexagonalNetwork(Network& S, Int N_x, Int N_y)->void;
        friend auto operator - (const Node&  n1, const Node&  n2) -> Length ;

        explicit Node  (const NetworkConfig& net_conf0, const NetworkTopologyConfig &topo_conf0, const ElementConfig config0)
        : GenericElement<Node, NodeState>(net_conf0,topo_conf0,config0) {}


        auto get_nodes_pores  () const ->  const std::vector<NodePore>&  { return nodePores; }

        auto set_type (NodeType t0)                  -> void      { type = t0;   }
        auto get_type () const                       -> NodeType  { return type; }
        auto set_pos  (Point3D p0)                   -> void      { pos = p0;    }
        auto get_pos  () const                       -> Point3D   { return pos;  }
        auto set_u    (Pressure u0)                  -> void      { state.u = u0;    }
        auto set_c    (SOLUBLES sp, Concentration c) -> void            { state.c[sp] = c;   }
        auto clear_c  ()                             -> void            { state.c.fill(0._C);}

        auto get_u    () const           -> Pressure        { ASSERT_MSG(state.u>=0._P,get_context_info());     return state.u;  }
        auto get_c    (SOLUBLES sp) const-> Concentration   { ASSERT_MSG(state.c[sp]>=0._C,get_context_info()); return state.c[sp];}


        //friend ofstream_ps_pores &operator <<(ofstream_ps_pores &stream, const Node &n){}     //TODO: implement it
        //friend ofstream_ps_grains &operator<<(ofstream_ps_grains &stream, const Node &n){}    //TODO: implement it



    protected:


        std::vector<NodePore> nodePores;
        NodeType type {NodeType::NORMAL};
        Point3D  pos  {Length(NaN),Length(NaN),Length(NaN)};


        void do_disconnect_from_network();

        auto do_init() -> void
        {
            for (auto sp : net_config.solubleS)
                if (type == NodeType::INPUT){
                    state.c[sp] = net_config.inlet_c[sp];
                }
                else
                    state.c[sp] = Concentration{0.0};
        }


        auto set_node_pores() -> void{
            auto it2 = nodes.begin();
            for(auto it = pores.begin(); it < pores.end() ; ++it) {
                if(it2==nodes.end()){
                    config.log.log<LogLevel::ERROR>("Problem with set_nodes_and_pores for ");
                    return;
                }
                nodePores.push_back({.n = (*it2), .p = (*it)});
                ++it2;
            }
        }


        auto do_is_state_set() -> bool{

            return (
                    !std::isnan(double(state.u)) and
                    std::ranges::all_of(state.c.get_data(), [](auto& pair) {
                        return double(pair.value) >= 0;
                    }) and
                            !isnan(pos)
            );
        }


        std::string do_get_state_info() const{
            std:: string str ="\t.u = " + state.u + "\t .type = " + type;
            for(auto sp : net_config.solubleS)
                str  += "\tc[" + sp + "] = "+state.c[sp];
            return str;
        }

    };


    inline auto operator - (const Node&  n1, const Node&  n2) -> Length {  //return distance between two points

        Length dx = abs(n1.pos.x - n2.pos.x);
        if(n1.topo_config.do_periodic_bc)
            if (dx > n1.topo_config.N_x*n1.net_config.l0 / 2.0)
                dx = n1.topo_config.N_x*n1.net_config.l0 - dx;

        Length dy = abs(n1.pos.y - n2.pos.y);
        if(n1.topo_config.do_periodic_bc and n1.topo_config.do_radial_geometry)
            if (dy > n1.topo_config.N_y*n1.net_config.l0 / 2.0)
                dy = n1.topo_config.N_y*n1.net_config.l0 - dy;

        Length dz = n1.pos.z - n2.pos.z;

        return sqrt(dx*dx + dy*dy + dz*dz);
    }



} // namespace karst

#endif //KARST_4_0_NODE_H
