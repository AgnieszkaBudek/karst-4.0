//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_GENERICELEMENT_H
#define KARST_4_0_GENERICELEMENT_H
#include "src/utils.h"
#include "src/units.h"
#include "src/chemistry/reaction_config.h"
#include "src/network/NetworkConfig.h"
#include "src/network/NetworkTopologyConfig.h"


namespace karst{

    /**
 * Generic class representing one module of models logics.
 * The module consists of Input and Output subclasses.
 * The module may, but doesn't have to be connected with particular symbol. In this case members sym_info and sym_params are filled.
 */
    class Network;
    class Node;
    class Pore;
    class Grain;

    enum class ElementType {
        NODE,
        PORE,
        GRAIN
    };
    // Specialization of EnumToString
    template<>
    const std::map<ElementType, std::string> EnumToString<ElementType>::mapping = {
            { ElementType::NODE,   "NODE"  },
            { ElementType::PORE,   "PORE"  },
            { ElementType::GRAIN,  "GRAIN" }
    };
//    // Operator << specialization for ElementType                 //TODO: wyrzucić potem
//    std::ostream& operator<<(std::ostream& os, ElementType value) {
//        return os << EnumToString<ElementType>::mapping.at(value);
//    }


    struct ElementConfig{

        const ElementType type;       ///< element type
        const Int name;               ///< element name
        std::ostream* log;            ///< log file
    };


    struct ElementTopoProperties{
        bool connected_to_percolation   {false};
        bool checked_for_percolation    {false};
        bool active                     {true};
    };


    template < typename Element, typename ElementState>
    class GenericElement
    {


    public:

        explicit GenericElement(const NetworkConfig& netconf0, const NetworkTopologyConfig &topo_conf0, const ElementConfig config0) :
                net_config{ netconf0 }, topo_config{ topo_conf0}, config{ config0}
        {}

        ~GenericElement() {
//            if(config.log)   //TODO: chack if this works
//                *(config.log)<<"Deleting element"<<*this;
        }



        friend auto createHexagonalNetwork(Network& S, Int N, Int M)->void;
        friend Network;

        // getting and updating element state
        inline auto set_state(ElementState&& s1)    ->  void { s=std::move(s1); }

        inline auto set_oldstate(ElementState&& s1) ->  void { s_old=std::move(s1); }

        inline auto update_state(ElementState& s1, long new_step) ->  void
        {
            assert (new_step>=step);
            s_old = std::move(s);
            s     = std::move(s1);
            step = new_step;
        }

        inline auto get_state() const             -> ElementState& { return s; }

        inline auto get_old_state() const         -> ElementState& { return s_old; }

        inline auto update_time_step(long step0)  -> void          { step = step0; }

        inline auto get_time_step () const        -> long          { return step; }


        //Saving info

        auto save_state() -> void {}

        auto save_topology() -> void {}

        auto log_state(const std::ostream& log_file)    ->  void {}

        auto log_topology(const std::ostream& log_file) ->  void {}


        //Setting the topology

        auto set_nodes  (std::deque<Node*  >&& n0) -> void { n = std::move(n0); }
        auto set_pores  (std::deque<Pore*  >&& p0) -> void { p = std::move(p0); }
        auto set_grains (std::deque<Grain* >&& g0) -> void { g = std::move(g0); }



        //checking the topology
        template <typename T>
        auto check_if_element_connected(const T& element, std::deque<const T*>& list_of_elements) ->bool {
            auto it = std::find_if(list_of_elements.begin(), list_of_elements.end(), [element](T* e) {
                return e->name == element.name;
            });
            return it != list_of_elements.end();
        }

        auto check_if_node_connected(const Node& n0) -> bool {
            return check_if_element_connected(n0,n);
        }

        auto check_if_pore_connected(const Pore& p0) -> bool {
            return check_if_element_connected(p0,p);
        }

        auto check_if_grain_connected(const Grain& g0) -> bool {
            return check_if_element_connected(g0,g);
        }

        //removing an element
        template <typename T>
        auto remove_element_from_connected(const T& element, std::deque<const T*>& list_of_elements) -> void {
            list_of_elements.erase(std::remove_if(list_of_elements.begin(), list_of_elements.end(), [element](T* e) {
                return e->name == element.name;
            }), list_of_elements.end());
        }

        auto remove_grain_from_connected(const Grain& g0) -> void {
            return remove_element_from_connected(g0,g);
        }

        auto remove_pore_from_connected(const Pore& p0) -> void {
            return remove_element_from_connected(p0,p);
        }

        auto remove_node_from_connected(const Node& n0) -> void {
            return remove_element_from_connected(n0,n);
        }

    public:

        std::deque<Node* >     n{};		///< list of nodes connected to the element
        std::deque<Pore* >     p{};		///< list of pores connected to the element
        std::deque<Grain* >    g{};		///< list of grains connected to the element

    protected:

        const NetworkConfig& net_config;   ///< NetworkConfig   //zmienićpotem na const Network* const S;
        const NetworkTopologyConfig& topo_config;

        const ElementConfig config;  ///< Config

        Long step{0};                ///< time step the element has been updated the last time
        ElementTopoProperties  x{};  ///< temporal properties of an element
        ElementState    s{};         ///< current state of an element
        ElementState    s_old{};     ///< state of an element in previous time step


    };

}



#endif //KARST_4_0_GENERICELEMENT_H
