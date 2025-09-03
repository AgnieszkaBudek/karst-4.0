//
// Created by Agnieszka on 23/05/2024.
//

#ifndef KARST_4_0_NETWORK_PRINTING_CONFIG_H
#define KARST_4_0_NETWORK_PRINTING_CONFIG_H

#include "src/utils.h"
#include "src/units.h"

namespace karst {



    struct PrintingConfig {

        PrintingConfig() = default;
        PrintingConfig(PrintingConfig&&) = default;
        PrintingConfig& operator=(PrintingConfig&&) = default;


        PrintingMode printing_mode  {PrintingMode::GRAIN_PRINT_M};      ///< printing network style
        Int s_save_data              {50};                ///< how often save txt and ps files (later will be automatized)
        Unitless print_diss_factor   {4.};			      ///< definition of dissolution pattern for printing; only pores with d>d0*print_diss_factor are printed
        Int initial_xy               {  };                ///< initial position of nodes: for printing in grains style
        Unitless pattern_anal_factor {2.};


        //output parameters
        bool do_save_ps            {true };    ///< if true ps pictures are created
        bool do_save_vtk           {true };    ///< if true data about network (nodes, pores and grains) is saved in text file
        bool do_save_txt           {true };    ///< if true save tables with diameters, flow and concentration
        bool do_save_data_in_vtk   {true };

        //printing pictures (as ps or pdf)
        Unitless L_out      {1. };			///< printing scale
        Int pages_tot       {100};		    ///< total nr of pages in the pictures (should be recalculated in future)
        Int pages_saved     {0  };	        ///< number of printed pages





    };

}

#endif //KARST_4_0_NETWORK_PRINTING_CONFIG_H
