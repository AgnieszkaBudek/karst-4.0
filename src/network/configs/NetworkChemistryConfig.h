//
// Created by Agnieszka on 23/05/2024.
//

#ifndef KARST_4_0_NETWORK_CHEMISTRY_CONFIG_H
#define KARST_4_0_NETWORK_CHEMISTRY_CONFIG_H

#include "src/utils.h"
#include "src/units.h"

namespace karst {


    struct NetworkChemistryConfig {

        std::map<ChemicalReaction,Velocity>     reaction_rate;              ///< reaction rate for precipitation
        std::map<SolubleChemical,Diffusion>     diffusion_rate;             ///< reaction rate for precipitation
        std::map<SolubleChemical,Diffusion>     transversal_diffusion_rate; ///< reaction rate for precipitation
        std::map<SolidChemical,Concentration>   capacity_nr;		        ///< capacity number for dissolution   (c_sol = 1 by default)
        std::map<SolubleChemical,Concentration> C_0 ;	            	    ///< inlet concentration of all soluble chamicals


    };

}

#endif //KARST_4_0_NETWORK_CHEMISTRY_CONFIG_H
