//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_GENERICELEMENT_H
#define KARST_4_0_GENERICELEMENT_H
#include "src/utils.h"
#include "src/units.h"


namespace karst{



    /**
 * Generic class representing one module of models logics.
 * The module consists of Input and Output subclasses.
 * The module may, but doesn't have to be connected with particular symbol. In this case members sym_info and sym_params are filled.
 */
    template < typename Element, typename ElementState>
    class GenericElement
    {


        enum ElementType {
            NODE,
            PORE,
            GRAIN
        };

        struct ElementConfig{

            const ElementType type;         ///< element type
            const Int name;                 ///< element name
            const std::ostream& log;        ///< log file
        };

        const ElementConfig config;


        struct ElementTopoProperties{
            bool connected_to_percolation   {false};
            bool checked_for_percolation    {false};
            bool active                     {false};
        };


    public:

        explicit GenericElement(const Network &S0, const Int name0, const std::ostream& log0, const ElementConfig& config0) :
                S{ S0 }, config{ config0}
        {}

        //virtual ~GenericElement() = default; //TODO Pamietac by sprawdzac czy nie pamietaja o nim inni


        // getting and updating element state


        auto get_state() -> const ElementState&
        {
            return s;
        }

        auto get_old_state() -> const ElementState&
        {
            return s_old;
        }

        auto update_state(const ElementState& s0) -> void
        {
            s = s0;
        }


        auto update_old_state(const ElementState & s0) -> void
        {
            s_old = s0;
        }

        auto update_time_step(long step0) -> void
        {
            step = step0;
        }


        //Saving info

        auto save_state() -> void {}

        auto save_topology() -> void {}

        auto log_state(const std::ostream& log_file) ->void {}

        auto log_topology(const std::ostream& log_file) ->void {}


        //Setting the topology

        auto set_nodes (const std::deque<Node* >& n0) -> void
        {
            n.assign(n0.begin(), n0.end());
        }

        auto set_pores (const std::deque<Pore* >& p0) -> void
        {
            p.assign(p0.begin(), p0.end());
        }

        auto set_grains (const std::deque<Grain* >& g0) -> void
        {
            g.assign(g0.begin(), g0.end());
        }



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



    protected:

        const Network& S;            ///< Network

        Long step{0};                ///< time step the element has been updated the last time
        ElementTopoProperties  x{};  ///< temporal properties of an element
        ElementState    s{};         ///< current state of an element
        ElementState    s_old{};     ///< state of an element in previous time step


        std::deque<const Node* >     n{};		///< list of nodes connected to the element
        std::deque<const Pore* >     p{};		///< list of pores connected to the element
        std::deque<const Grain* >    g{};		///< list of grains connected to the element


    };

}



#endif //KARST_4_0_GENERICELEMENT_H
