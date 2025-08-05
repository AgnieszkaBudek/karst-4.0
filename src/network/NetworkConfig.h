//
// Created by Agnieszka on 23/05/2024.
//

#ifndef KARST_4_0_NETWORK_CONFIG_H
#define KARST_4_0_NETWORK_CONFIG_H

#include "src/utils.h"
#include "src/units.h"


namespace karst {


    struct NetworkConfig {

        NetworkConfig() = default;
        NetworkConfig(NetworkConfig&&) = default;
        NetworkConfig& operator=(NetworkConfig&&) = default;

        // Main Network Parameters

        Length   l0   {1.0};	///< (initial) characteristic pore length (should be always equal to one!!!!)
        Length   d0   {0.1};	///< (initial) characteristic pore diameter

        Flow     Q_tot {0.};      ///< total Flow through the system
        Pressure P_in  {0.};      ///< inlet pressure
        Pressure P_out {0.};      ///< outlet pressure


        Unitless Da   {1.0};	///< effective Damkohler number for first reaction
        Unitless DaPe {1.0};	///< DaPe for first reaction (previous G, Diffusion across the pore)
        Unitless Pe   {0.0};	///< Peclet number for first reaction (D along pore)

        EnumArray <SOLIDS, Unitless,enum_size_v<SOLIDS>>    gamma {1._U*NaN};	///< ratio of acid capacity numbers between first and the rest of reactions
        EnumArray<REACTION,Unitless,enum_size_v<REACTION>>  kappa {1._U*NaN};	///< ratio of Da_1/Da_2 = ratio of reaction rates
        EnumArray<REACTION,Unitless,enum_size_v<REACTION>>  theta {1._U*NaN};	///< ratio of DaPe/DaPe
        EnumArray<REACTION,Unitless,enum_size_v<REACTION>>  lambda{1._U*NaN}; 	///< ratio of Pe (Diffusion along the pore)

        Length d_min {0.001};    ///< minimal possible pore diameter (important in precipitation)
        Length l_min {1e-10};    ///< minimal possible pore length (must be >0 for numerical reasons)
        Length h_tot {1.0};      ///< total length of the system
        Double l_V_scaling_f {1./3};

        //physical parameters -> should be set after choosing dimensionless one

        Unitless Sh     {4.0};	      ///< Sherwood number for pipe
        Viscosity mu_0  {0.0};  	  ///< viscosity  always set to M_PI*pow(d0,4)/(128*l0)
        Time dt_unit    {0.0};        ///< time unit (in dimensionless units [d0/2 k1 * gamma_1])

        // Reactions
        ReactionSet                              reaction_set  = ReactionSet::LINEAR_DP;
        std::array<SOLUBLES,2>                   solubleS      = {SOLUBLES::B,SOLUBLES::C};
        std::array<SOLIDS,2>                     solidS        = {SOLIDS::A,SOLIDS::E};
        EnumArray<SOLUBLES,Diffusion,enum_size_v<SOLUBLES>>           diffusion_rate;              ///< reaction rate for precipitation
        EnumArray<SOLUBLES,Diffusion,enum_size_v<SOLUBLES>>           transversal_diffusion_rate;  ///< reaction rate for precipitation
        EnumArray<SOLUBLES,Concentration,enum_size_v<SOLUBLES>>       inlet_c = {
                                                    {SOLUBLES::B,1._C},
                                                    {SOLUBLES::C,0._C}};   ///< capacity number for reactions  //TODO: to musi byćdopiero wczytywane z pliku konfiguracyjnego

    };



} //namespace karst
#endif //KARST_4_0_NETWORK_CONFIG_H
