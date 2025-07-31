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
 * Generic class for material network element: pore, grain or node
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


    struct ElementConfig{
        const ElementType type;       ///< element type
        const Int name;               ///< element name
        int a_name{-1};               ///< element name for active subset
        std::ostream* log;            ///< log file
    };


    struct ElementTopoState{
        bool connected_to_percolation_cluster   {false};
        bool checked_for_percolation            {false};
    };



    template < typename Element, typename ElementState>
    class GenericElement
    {

    public:

        friend auto createHexagonalNetwork(Network& S, Int N_x, Int N_y)->void;
        friend Network; friend Node; friend Pore; friend Grain;


        explicit GenericElement(const NetworkConfig& netconf0,
                                const NetworkTopologyConfig& topo_conf0,
                                const ElementConfig config0) :
                                net_config{ netconf0 },
                                topo_config{ topo_conf0},
                                config{ config0}
        {}



        ~GenericElement() {
           // std::cerr<<"Deleting element: "<<config.type<<" -> "<<config.name<<std::endl;
        }

        auto remove_element_from(std::vector<Element*> elV) -> void{
            erase_if(elV,[this](auto x)->bool{return x==this;});
        }


        auto remove_element_from_a_network() -> void{
            static_cast<Element *>(this)->do_disconnect_from_network();
            pores.clear();
            nodes.clear();
            grains.clear();
//            state={};
        }


        auto init() -> void { static_cast<Element*>(this)->do_init();}

        // getting and updating element state

        auto set_state     (ElementState&& s1)               ->  void { state=std::move(s1); }
        auto set_old_state (ElementState&& s1)               ->  void { s_old=std::move(s1); }
        auto update_old_state()                              ->  void {s_old = state;}
        auto revert_state()                                  ->  void {state = s_old;}
        auto update_state  (ElementState& s1, long new_step) ->  void {
            assert (new_step>=step);
            s_old = std::move(state);
            state = std::move(s1);
            step  = new_step;
        }

        // State access
        auto get_state() const             -> ElementState& { return state; }
        auto get_old_state() const         -> ElementState& { return s_old; }
        auto update_time_step(long step0)  -> void          { step = step0; }
        auto get_time_step () const        -> long          { return step; }
        auto is_state_set() -> bool{ static_cast<Element*>(this)->do_is_state_set();}

        //Topo_config access
        auto is_connected_to_percolation_cluster() const -> bool { return topo_s.connected_to_percolation_cluster; }
        auto is_checked_for_percolation()          const -> bool { return topo_s.checked_for_percolation; }
        auto set_connected_to_percolation_cluster()      -> void { topo_s.connected_to_percolation_cluster = true; }
        auto set_checked_for_percolation()               -> void { topo_s.checked_for_percolation = true; }
        auto clean_percolation_state()                   -> void { topo_s={false,false};}

        //Saving info
        auto save_state()    -> void {}
        auto save_topology() -> void {}
        auto log_state(const std::ostream& log_file)    ->  void {}
        auto log_topology(const std::ostream& log_file) ->  void {}


        //Setting the topology
        auto set_nodes  (std::vector<Node*  >&& n0) -> void { nodes = std::move(n0); }
        auto set_pores  (std::vector<Pore*  >&& p0) -> void { pores = std::move(p0); }
        auto set_grains (std::vector<Grain* >&& g0) -> void { grains = std::move(g0); }

        auto get_nodes  () const ->  const std::vector<Node*  >&  { return nodes; }
        auto get_pores  () const ->  const std::vector<Pore*  >&  { return pores; }
        auto get_grains () const ->  const std::vector<Grain* >&  { return grains; }

        auto check_if_active () const -> const bool {return active;}

        auto deactivate () -> void {
            active = false;
            remove_element_from_a_network();
        }


        //checking the topology
        template <typename T>
        auto check_if_element_connected(const T& element, std::vector<const T*>& list_of_elements) ->bool {
            auto it = std::find_if(list_of_elements.begin(), list_of_elements.end(), [element](T* e) {
                return e->name == element.name;
            });
            return it != list_of_elements.end();
        }


        const NetworkConfig& net_config;               ///< NetworkConfig   //zmienić potem na const Network* const S;
        const NetworkTopologyConfig& topo_config;
        ElementConfig config;                  ///< Config


    protected:

        std::vector<Node*>     nodes{};		///< list of nodes connected to the element
        std::vector<Pore*>     pores{};		///< list of pores connected to the element
        std::vector<Grain*>    grains{};    ///< list of grains connected to the element

        bool active{true};               ///< if the element is still part of the network
        Long step{0};                    ///< time step the element has been updated the last time

        ElementTopoState  topo_s{}; ///< temporal properties of an element
        ElementState      state{};         ///< current state of an element
        ElementState      s_old{};         ///< state of an element in previous time step


    };

}



#endif //KARST_4_0_GENERICELEMENT_H
