//
// Created by Agnieszka on 23/05/2024.
//

#ifndef KARST_4_0_NETWORK_CONFIG_H
#define KARST_4_0_NETWORK_CONFIG_H

#include "src/utils.h"
#include "src/units.h"
#include "NetworkTopologyConfig.h"


namespace karst {


    struct NetworkConfig {

        NetworkConfig() = default;
        NetworkConfig(NetworkConfig&&) = default;
        NetworkConfig& operator=(NetworkConfig&&) = default;

        // Main Network Parameters

        Length l0 {1.};		    ///< (initial) characteristic pore length (should be always equal to one!!!!)
        Length d0 {0.1};	    ///< (initial) characteristic pore diameter
        Unitless Da {1.};		///< effective Damkohler number for first reaction
        Unitless DaPe {1.};		///< DaPe for first reaction (previous G, Diffusion across the pore)
        Unitless Pe {0.};		///< Peclet number for first reaction (D along pore)

        Flow Q_tot     {0.};      ///< total Flow through the system
        Pressure P_in  {0.};      ///< inlet pressure
        Pressure P_out {0.};      ///< outlet pressure

        std::map<SPECIES,Unitless>  gamma ;	            ///< ratio of acid capacity numbers between first and the rest of reactions
        std::map<ChemicalReaction,Unitless>  kappa ;	///< ratio of Da_1/Da_2 = ratio of reaction rates
        std::map<ChemicalReaction,Unitless>  theta ;	///< ratio of DaPe/DaPe
        std::map<ChemicalReaction,Unitless>  lambda; 	///< ratio of Pe (Diffusion along the pore)

        Length d_min {0.001};    ///< minimal possible pore diameter (important in precipitation)
        Length l_min {1e-10};    ///< minimal possible pore length (must be >0 for numerical reasons)



        //physical parameters -> should be set after choosing dimenssionless one

        Unitless Sh     {4.0};	      ///< Sherwood number for pipe
        Viscosity mu_0  {0.0};  	  ///< viscosity  always set to M_PI*pow(d0,4)/(128*l0)
        Time dt_unit    {0.0};        ///< time unit (in dimensionless units [d0/2 k1 * gamma_1])


        std::map<ChemicalReaction,Velocity>   reaction_rate;               ///< reaction rate for precipitation
        std::map<SPECIES,Diffusion>           diffusion_rate;              ///< reaction rate for precipitation
        std::map<SPECIES,Diffusion>           transversal_diffusion_rate;  ///< reaction rate for precipitation
        std::map<SPECIES,Concentration>       inlet_c;  		           ///< capacity number for reactions




    };



} //namespace karst
#endif //KARST_4_0_NETWORK_CONFIG_H
