//
// Created by Agnieszka on 23/05/2024.
//

#ifndef KARST_4_0_NETWORK_STATE_H
#define KARST_4_0_NETWORK_STATE_H

#include "src/utils.h"
#include "src/units.h"

namespace karst {


    struct NetworkState {



        // flow parameter
        Pressure P_in  {10.};     ///< pressure at the inlet (must by positive)
        Pressure P_out {0.};	     ///< pressure at the outlet, always should be set to zero
        Flow     Q_tot {10.};     ///< total flow through the system (if == 0 the constant pressure is kept)

        Long     N_active{0};               ///<number_of_active_nodes
        Long     N_active_connections{0};   ///<number_of_active_nodes
        bool     new_Topo{false};           ///< Give info if the topology has been updated

        // chemical properties
        EnumArray <SOLUBLES, Concentration, enum_size_v<SOLUBLES>>   C_0   {0._C} ;	  ///< inlet concentration of all soluble chemicals
        EnumArray <SOLIDS,  Volume,         enum_size_v<SOLIDS>>     V_tot {0._V};    ///< total volume of all species

    };

}

#endif //KARST_4_0_NETWORK_STATE_H
