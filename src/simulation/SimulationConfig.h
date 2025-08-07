//
// Created by Agnieszka on 23/05/2024.
//

#ifndef KARST_4_0_SIMULATION_CONFIG_H
#define KARST_4_0_SIMULATION_CONFIG_H

#include "src/utils.h"
#include "src/units.h"

namespace karst {

    enum class INTEGRATION_METHOD  {EULER, LEAP_FROG};

    struct SimulationConfig {

        SimulationConfig() = default;
        SimulationConfig(SimulationConfig&&) = default;
        SimulationConfig& operator=(SimulationConfig&&) = default;

        //evolution parameters
        Time T_max       {1.};     	    ///< maximal number of time steps

        Long tot_steps   {0};           ///< total nr of steps in simulation
        Time tot_time    {0.0};         ///< total time in simulation
        Time dt0         {0.01};        ///< initial time step

        Length d_d_max   {0.1};	        ///< maximal change of pore diameter in one step (in %; if obtained then dt = 2/3 dt)
        Length d_d_min   {0.01};	    ///< minimal change of pore diameter in one step (in %; if not obtained then dt = 1.2dt)
        Unitless d_d_dis {4.0};         ///< minimal dissolution at the outlet to finish simulation (unit d0)
        Pressure u_min   {0.001};       ///< minimal pressure drop to finish simulation.
        Volume d_V_min   {0.01};        ///< minimal change of pore volume in one step (if not obtained the dt = 1.2dt)
        Volume d_V_max   {0.1};         ///< maximal change of pore volume in one step (if obtained the dt = 2/3dt)

        Unitless time_factor_after_b {1.5};                  ///< how long after breakthrough run simulation (as a fraction fof tot_steps)

        //dynamics
        INTEGRATION_METHOD integration_mode       {INTEGRATION_METHOD::EULER};
        bool do_leapfrog                          {false};       ///< if true frog leap instead of Euler algorithm is used in evolution (not implemented yet) //TODO: get rid of ot later
        bool do_full_dissolution                  {true};        ///< if true evolution stops when system is fully dissolved
        bool do_system_dissolved                  {false};       ///< check if system is dissolved (fulfilling condition given by d_d_diss)
        bool do_adaptive_dt                       {true}; 	     ///< adapting dt according to d_d_max and d_d_min;
        bool do_recalculate_physical_parameters   {true};        ///< if true recalculate physical parameters according to dimensionless one
        bool do_smarter_calculation_of_pressure   {false};       ///< if true pressure and flow is calculate in two steps
        bool do_dynamical_length				  {true};		 ///< if true length of pore is changing according to dissolution and precipitation
        bool do_cut_d_min                         {true};        ///< if true the d<=d_min -> d=0

        Length d_max_for_u {1000};  ///< maximal diameter that consume pressure, for d>d_max_for_u * d0 delta u in pore is zero
    };

}

#endif //KARST_4_0_SIMULATION_CONFIG_H
