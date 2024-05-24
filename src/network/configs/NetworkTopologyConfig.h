//
// Created by Agnieszka on 23/05/2024.
//

#ifndef KARST_4_0_NETWORK_TOP_CONFIG_H
#define KARST_4_0_NETWORK_TOP_CONFIG_H

#include "src/utils.h"
#include "src/units.h"

namespace karst {


    struct NetworkTopologyConfig {

        enum TypeOfNetTopology  {HEXAGONAL_TOP, TOP_FROM_FILE, TOP_FROM_TRIANGULARIZATION};

        Int N_x {10};		///< size of regular network
        Int N_y {10};		///< size of regular network
        Int N_z {0};        ///< size of regular network

        //type of network
        TypeOfNetTopology type_of_topology   {TOP_FROM_TRIANGULARIZATION};
        std::string in_topology_file_name    {"net_0.out"};     ///< file name with input topology of the network
        std::string in_topology_file_name_g  {"net_g_0.out"};   ///< file name with input topology of the network
        std::string in_pore_size_file_name   {"pores_0.out"};   ///<file name with input pore sizes
        bool do_radial_geometry           {false};          ///< if true (false is default) one input in the centre and output along circle
        bool do_randomness_in_regular_net {true};  	        ///< if true randomness is added to hexagonal network (working for hexagonal net)
        bool do_clear_unused_pores        {true};           ///< if true unused pores and nodes and grains are deleted
        bool do_periodic_bc			      {true};	        ///< if periodic boundary condition
        Int random_seed {-1};
        Length gauss_sigma_d {0};    	  ///< if randomness is on this give information about width of the initial diameter distribution (log normal used here)
        Length max_rand_shift_xy {1};     ///< if randomness is on this give information about max shift in positions


    };

}

#endif //KARST_4_0_NETWORK_TOP_CONFIG_H
