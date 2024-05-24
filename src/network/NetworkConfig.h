//
// Created by Agnieszka on 23/05/2024.
//

#ifndef KARST_4_0_NETWORK_CONFIG_H
#define KARST_4_0_NETWORK_CONFIG_H

#include "src/utils.h"
#include "src/units.h"
#include "src/network/configs/NetworkChemistryConfig.h"
#include "src/network/configs/NetworkTopologyConfig.h"
#include "src/network/configs/NetworkPrintingConfig.h"


namespace karst {


    struct NetworkConfig {

        NetworkChemistryConfig chem_config  {};
        NetworkTopologyConfig  topo_config  {};
        NetworkPrintingConfig  print_config {};



        enum TypeOfMerging      {MERGING_NONE, MERGING_GRAINS};
        enum PrintingMode       {DEBUGGING_PRINT_M, GRAIN_PRINT_M};



        // Main Network Parameters

        Length l0 {1};		    ///< initial characteristic pore length (should be always equal to one!!!!)
        Length d0 {0.1};	    ///< initial characteristic pore diameter
        Unitless Da {1};		///< effective Damkohler number for first reaction
        Unitless DaPe {1};		///< DaPe for first reaction (previous G, Diffusion across the pore)
        Unitless Pe {1};		///< Peclet number for first reaction (D along pore)


        std::map<SolidChemical,Unitless>    gamma ;	    ///< ratio of acid capacity numbers between first and the rest of reactions
        std::map<ChemicalReaction,Unitless> kappa ;	    ///< ratio of Da_1/Da_2 = ratio of reaction rates
        std::map<SolubleChemical,Unitless>  theta ;	    ///< ratio of DaPe/DaPe
        std::map<SolubleChemical,Unitless>  lambda ;	///< ratio of Pe (Diffusion along the pore)

        Length d_min {0.001};    ///< minimal possible pore diameter (important in precipitation)
        Length l_min {1e-10};    ///< minimal possible pore length (must be >0 for numerical reasons)



        //physical parameters -> should be set after choosing dimenssionless one


        Unitless Sh     {4};	///< Sherwood number for pipe
        Viscosity mu_0  {0};	///< viscosity  always set to M_PI*pow(d0,4)/(128*l0)
        Time dt_unit    {0};    ///< time unit (in dimensionless units [d0/2 k1 * gamma_1])


        //evolution parameters
        Long T_max       {10};     	    ///< maximal number of time steps

        Long tot_steps   {0};           ///< total nr of steps in simulation
        Time tot_time    {0};           ///< total time in simulation

        Length d_d_max   {0.1};	        ///< maximal change of pore diameter in one step (in %; if obtained then dt = 2/3 dt)
        Length d_d_min   {0.01};	    ///< minimal change of pore diameter in one step (in %; if not obtained then dt = 1.2dt)
        Unitless d_d_dis {4.0};         ///< minimal dissolution at the outlet to finish simulation (unit d0)
        Pressure u_min   {0.001};       ///< minimal pressure drop to finish simulation.
        Volume d_V_min   {0.01};        ///< minimal change of pore volume in one step (if not obtained the dt = 1.2dt)
        Volume d_V_max   {0.1};         ///< maximal change of pore volume in one step (if obtained the dt = 2/3dt)


        Length d_max_for_u {1000};                           ///< maximal diameter that consume pressure, for d>d_max_for_u * d0 delta u in pore is zero
        Unitless time_factor_after_b {1.5};                  ///< how long after breakthrough run simulation (as a fraction fof tot_steps)

        //merging parameters
        TypeOfMerging type_of_merging {MERGING_NONE}; ///< type of merging


        //printing pictures

        Unitless L_out      {1};			///< printing scale
        Int pages_tot       {100};		    ///< total nr of pages in the pictures (should be recalculated in future)
        Int pages_saved     {0};	        ///< number of printed pages

        PrintingMode printing_mode {GRAIN_PRINT_M};       ///< printing network style
        Int s_save_data         {50};                     ///< how often save txt and ps files (later will be automatized)
        Int print_diss_factor   {4};					  ///< definition of dissolution pattern for printing; only pores with d>d0*print_diss_factor are printed
        Int initial_xy          = NULL;                   ///< initial position of nodes: for printing in grains style
        Int pattern_anal_factor {2};




        //dynamics
        do_leapfrog                          = false;        //if true frog leap instead of Euler algorithm is used in evolution (not implemented yet)
        do_full_dissolution                  = true;         //if true evolution stops when system is fully dissolved
        do_system_dissolved                  = false;        //check if system is dissolved (fulfilling condition given by d_d_diss)
        sim_state                            = 1;            // 1-ok, 2-warningi, minus - after breackthrough
        if_adaptive_dt                       = true; 	     //adapting dt according to d_d_max and d_d_min;
        if_recalculate_physical_parameters   = true;         //if true recalculate physical parameters according to dimensionless one
        if_smarter_calculation_of_pressure   = true;         //if true pressure and flow is calculate in two steps
        if_precipitation				     = false;		 //if true apart from dissolution the precipitation in on
        if_dynamical_length				     = true;		 //if true length of pore is changing according to dissolution and precipitation
        if_streamtube_mixing                 = false;        //if true the stream-tube mixing is perform while calculation the concentration (works only for dissolution now)
        if_cut_d_min                         = false;        //if true the d<=d_min -> d=0

        if_dynamic_k2                        = false;        // if true the Da2_eff will be multiplied by sigma function
        dyn_k2_alpha                         = 2;            //sigma parameters
        dyn_k2_c0                            = 0.1;          //sigma parameters


        //output
        if_save_ps            = true;     //if true ps pictures are created
        if_save_txt           = true;     //if true data about network (nodes, pores and grains) is saved in text file
        if_save_table         = true;     //if true save tables with diameters, flow and concentration
        if_save_topology      = true; 	  //if true topology is saved in each save_all
        if_verbose            = false;    //if true verbose version for debugging
        if_debugging_printing = false;	  //if true debugging printing is done after each calculation

        //addition inlet cut
        inlet_cut_factor = 1;      //factor of an inlet cut (in a cut: d = d*factor)
        inlet_cut_w = 0;           //width of an inlet cut
        inlet_cut_l = 0;		   //length of aGn inlet cut
        add_well    = false;       //if true additional cut will be added
        point_inlet = false;	   // if true the inlet is a point at the top of the crack
        point_outlet = false;	   // if true the outlet is a point at the top of the crack





    };




    auto read_network_config_from_file(std::istream& cfile) -> const NetworkConfig&
    {
        static NetworkConfig config{
                //default values

                .N_x = 10;		//size of regular network
                .N_y = 10;		//size of regular network

                .P_in   = N_y-1;   //pressure at the inlet (must by positive)
                .P_out  = 0;	      //pressure at the outlet, always should be set to zero
                .Q_tot  = 2*N_x;   //total flow through the system (if == 0 the constant pressure is kept)
                .Va_tot = 0;       //total volume of dissolving species
                .Ve_tot = 0;       //total volume of precipitating species
                .Vx_tot = 0;       //total amount of non reacting species
                .Vx_perc= 0;       // percentage of non reacting species in the system


                //dimenssionless parameters describing evolution of the system

                l0    = 1;		 //initial characteristic pore length (should be always equal to one!!!!)
                d0    = 0.1*l0;	 //initial characteristic pore diameter
                Da    = 1;		 //effective Damkohler number for dissolution
                Da2   = 1;		 //effective Damkohler number for precipitation
                G1    = 1;		 //DaPe for dissolution
                G2    = 1;		 //DaPe for precipitation
                Pe1   = 1;		 //Peclet number for dissolution (D along pore)
                Pe2   = 1;		 //Peclet number for precipitation (D along pore)
                gamma = 1;	     //ratio of acid capacity numbers between dissolution and precipitation (gamma1/gamma2)
                kappa = 1;	     //ratio of Da_1/Da_2 = ratio of reaction rates
                theta = 1;	     //ratio of G_1/G_2
                d_min = d0/100.; //minimal possible pore diameter (important in precipitation)
                l_min = l0*1e-10;//minimal possible pore length (must be >0 for numerical reasons)



                //physical parameters -> should be set after choosing dimenssionless one

                k1	= 10e-8;		//reaction rate for dissolution
                k2	= 1;			//reaction rate for precipitation
                D1	= 0;			//diffusion coefficient for dissolution
                D2	= 1;			//diffusion coefficient for precipitation
                DD1 =-1;			//transversal diffusion coefficient for dissolution
                DD2 = 1;			//transversal diffusion coefficient for precipitation
                Sh  = 4;			//Sherwood number for pipe
                gamma_1	= 1;		//capacity number for dissolution   (c_sol = 1 by default)
                gamma_2 = 1;		//capacity number for precipitation (c_sol = 1 by default)
                Cb_0 	= 1;		//acid inlet concentration
                Cc_0	= 0;		//precipitating species inlet concentration
                mu_0    = M_PI*pow(d0,4)/(128*l0);		//viscosity  always set to M_PI*pow(d0,4)/(128*l0)
                dt_unit = 2*k1 * gamma_1/d0;            //(in dimensionless units [2 k1 * gamma_1/d0])

                //evolution parameters
                T_max       = 10;     	  //maximal number of time steps
                sim_step    = 0;          //nr of simulation steps already done
                tot_steps   = 0;          //total nr of steps in simulation
                tot_time    = 0;          //total time in simulation
                dt          = 1;      	  //time step (in dimensionless units [2 k1 * gamma_1/d0])
                d_d_max     = 0.1;	      //maximal change of pore diameter in one step (in %; if obtained then dt = 2/3 dt)
                d_d_min     = 0.01;	      //minimal change of pore diameter in one step (in %; if not obtained then dt = 1.2dt)
                d_d_dis     = 4;          //minimal dissolution at the outlet to finish simulation (unit d0)
                u_min       = 0.001;      ///< minimal pressure drop to finish simulation.
                d_V_min     = 0.01;       //(only for precipitation)  minimal change of pore volume in one step (if not obtained the dt = 1.2dt)
                d_V_max     = 0.1;        //(only for precipitation)  maximal change of pore volume in one step (if obtained the dt = 2/3dt)
                set_new_dt  =-1;          //0-don't change dt; 1 - increase dt by factor 1.2; -1 - decrease dt by factor 0.75
                d_max_for_u = 1000;       //maximal diameter that consume pressure, for d>d_max_for_u * d0 delta u in pore is zero
                time_factor_after_b = 1.5;  //how long after breakthrough run simulation (as a fraction fof tot_steps)

                //merging parameters
                type_of_merging = "merge_empty_grains"; //type of merging: "none", "merge_empty_grains", not implemented yet: "merge_pores"


                //printing pictures
                L_out               = 1;			          //printing scale
                pages_tot           = 100;		          //total nr of pages in the pictures (should be recalculated in future)
                pages_saved         = 0;	                  //number of printed pages
                description_note    = "Default network.";   //title note in ps/pdf files
                printing_mode       = "debugging";            //printing network style
                s_save_data         = 50;                    //how often save txt and ps files (later will be automatized)
                print_diss_factor   = 4;					   //definition of dissolution pattern for printing; only pores with d>d0*print_diss_factor are printed
                initial_xy          = NULL;                   //initial position of nodes: for printing in grains style
                pattern_anal_factor = 2;

//CONTROL PARAMETERS
                //type of network
                type_of_topology                   = "from_file";    //three options: "hexagonal", "from_file", "triangularization"
                in_topology_file_name              = "net_0.out";    //file name with input topology of the network
                in_topology_file_name_g            = "net_g_0.out";  //file name with input topology of the network
                in_pore_size_file_name             = "pores_0.out";  //file name with input pore sizes
                if_radial_geometry                 = false;          // if true (false is default) one input in the centre and output along circle
                if_randomness_in_regular_net       = true;  	     //if true randomness is added to hexagonal network (working for hexagonal net)
                if_clear_unused_pores              = true;           //if true unused pores and nodes and grains are deleted
                if_track_grains                    = true;           //if true grains and their volume are tracked (important for merging and precipitation)
                if_periodic_bc					   = true;	         //if periodic boundary condition
                random_seed                        = -1;
                gauss_sigma_d                      = 0;    			 //if randomness is on this give information about width of the initial diameter distribution (log normal used here)
                max_rand_shift_xy                  = 1;       	     //if randomness is on this give information about max shift in positions



                //dynamics
                if_leapfrog                          = false;        //if true frog leap instead of Euler algorithm is used in evolution (not implemented yet)
                if_full_dissolution                  = true;         //if true evolution stops when system is fully dissolved
                if_system_dissolved                  = false;        //check if system is dissolved (fulfilling condition given by d_d_diss)
                sim_state                            = 1;            // 1-ok, 2-warningi, minus - after breackthrough
                if_adaptive_dt                       = true; 	     //adapting dt according to d_d_max and d_d_min;
                if_recalculate_physical_parameters   = true;         //if true recalculate physical parameters according to dimensionless one
                if_smarter_calculation_of_pressure   = true;         //if true pressure and flow is calculate in two steps
                if_precipitation				     = false;		 //if true apart from dissolution the precipitation in on
                if_dynamical_length				     = true;		 //if true length of pore is changing according to dissolution and precipitation
                if_streamtube_mixing                 = false;        //if true the stream-tube mixing is perform while calculation the concentration (works only for dissolution now)
                if_cut_d_min                         = false;        //if true the d<=d_min -> d=0

                if_dynamic_k2                        = false;        // if true the Da2_eff will be multiplied by sigma function
                dyn_k2_alpha                         = 2;            //sigma parameters
                dyn_k2_c0                            = 0.1;          //sigma parameters


                //output
                if_save_ps            = true;     //if true ps pictures are created
                if_save_txt           = true;     //if true data about network (nodes, pores and grains) is saved in text file
                if_save_table         = true;     //if true save tables with diameters, flow and concentration
                if_save_topology      = true; 	  //if true topology is saved in each save_all
                if_verbose            = false;    //if true verbose version for debugging
                if_debugging_printing = false;	  //if true debugging printing is done after each calculation

                //addition inlet cut
                inlet_cut_factor = 1;      //factor of an inlet cut (in a cut: d = d*factor)
                inlet_cut_w = 0;           //width of an inlet cut
                inlet_cut_l = 0;		   //length of aGn inlet cut
                add_well    = false;       //if true additional cut will be added
                point_inlet = false;	   // if true the inlet is a point at the top of the crack
                point_outlet = false;	   // if true the outlet is a point at the top of the crack


        }


    }

} //namespace karst
#endif //KARST_4_0_NETWORK_CONFIG_H
