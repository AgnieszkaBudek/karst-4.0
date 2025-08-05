//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_REACTIONCONFIG_H
#define KARST_4_0_REACTIONCONFIG_H

#include "src/utils.h"
#include "src/units.h"

namespace karst{

    enum class SOLUBLES {B, C, D, SIZE};
    template<>
    const std::map<SOLUBLES, std::string> EnumToString<SOLUBLES>::mapping = {
            { SOLUBLES::B, "B" },
            { SOLUBLES::C, "C" },
            { SOLUBLES::D, "D" },
    };

    enum class SOLIDS {A, E, X, SIZE};
    template<>
    const std::map<SOLIDS, std::string> EnumToString<SOLIDS>::mapping = {
            { SOLIDS::A, "A" },
            { SOLIDS::E, "E" },
            { SOLIDS::X, "X" },
    };


    enum class REACTION {D, P, SIZE};
    template<>
    const std::map<REACTION, std::string> EnumToString<REACTION>::mapping = {
            {REACTION::D, "DISSOLUTION" },
            {REACTION::P, "PRECIPITATION" },

    };


    struct ChemicalReaction{
        std::string name;
        std::vector<SOLIDS>    tracked_solids;
        std::vector<SOLUBLES>  tracked_solubles;

        Velocity k{NaN};        // reaction rate
        Concentration c_eq{NaN};
        Unit<std::ratio<0>,std::ratio<3>,std::ratio<0>,std::ratio<-1>> solid_mol_volume{NaN};
        //const double K;             // reaction coefficient1
        //const double O;             // reaction coefficient2
    };




    }



#endif //KARST_4_0_REACTIONCONFIG_H
