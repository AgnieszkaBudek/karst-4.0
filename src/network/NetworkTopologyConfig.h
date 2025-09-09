//
// Created by Agnieszka on 23/05/2024.
//

#ifndef KARST_4_0_NETWORK_TOP_CONFIG_H
#define KARST_4_0_NETWORK_TOP_CONFIG_H

#include "src/utils.h"
#include "src/units.h"

namespace karst {


    struct NetworkTopologyConfig {

        NetworkTopologyConfig() = default;
        NetworkTopologyConfig(NetworkTopologyConfig&&) = default;
        NetworkTopologyConfig& operator=(NetworkTopologyConfig&&) = default;

        Int N_x {4};		///< size of regular network
        Int N_y {4};		///< size of regular network
        Int N_z {0};        ///< size of regular network

        //type of the network
        TypeOfNetTopology type_of_topology   {TypeOfNetTopology::CUBIC};
        std::string in_topology_file_name    {"net_0.vtu"};       ///< file name with input topology of the network
        //read form csv files
        std::string nodes_csv = "/Users/agnieszka/Desktop/KARST/DATA/KOREAN_ROCK/sample_1/nodes.csv";
        std::string pores_csv = "/Users/agnieszka/Desktop/KARST/DATA/KOREAN_ROCK/sample_1/pores.csv";

        //randomness in the network
        Length min_distance_between_nodes {0.5};
        Int random_seed {21};                        ///< seed for generating randomness in the network
        Length gauss_sigma_d {0};    	             ///< if randomness is on this give information about width of the initial diameter distribution (log normal used here)
        bool do_randomness_in_regular_net {false};  	 ///< if true randomness is added to hexagonal network (working for hexagonal net)
        Unitless max_rand_shift {0.2};               ///< if randomness is on this give information about max shift in positions (x l_0)

        //boundary conditions
        bool do_periodic_bc	       {true};	     ///< if periodic boundary condition
        bool do_clear_unused_net_el{true};       ///< if true unused pores and nodes and grains are deleted
        bool do_radial_geometry    {false};      ///< if true (false is default) one input in the centre and output along circle
        bool point_inlet           {false};	     ///< if true the inlet is a point at the top of the crack
        bool point_outlet          {false};	     ///< if true the outlet is a point at the top of the crack


        //merging parameters
        TypeOfMerging type_of_merging {TypeOfMerging::MERGING_NONE}; ///< type of merging


        //addition inlet cut/fracture
        Unitless inlet_cut_factor {1};      ///< factor of an inlet cut (in a cut: d = d*factor) if 1 no cut is done
        Int inlet_cut_w           {0};      ///< width of an inlet cut if 0 the fracture is done
        Int inlet_cut_l           {N_y};    ///< length of an inlet cut

    };

}

#endif //KARST_4_0_NETWORK_TOP_CONFIG_H
