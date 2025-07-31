//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_REACTIONCONFIG_H
#define KARST_4_0_REACTIONCONFIG_H

#include "src/utils.h"
#include "src/units.h"

namespace karst{

    enum class SPECIES {A, B, C, E};
    template<>
    const std::map<SPECIES, std::string> EnumToString<SPECIES>::mapping = {
            { SPECIES::A, "A" },
            { SPECIES::B, "B" },
            { SPECIES::C, "C" },
            { SPECIES::E, "E" },
    };

    enum class REACTIONS {D,P};
    template<>
    const std::map<REACTIONS, std::string> EnumToString<REACTIONS>::mapping = {
            { REACTIONS::D, "DISSOLUTION" },
            { REACTIONS::P, "PRECIPITATION" },

    };


    struct ChemicalReaction{
        std::string name;
        std::vector<SPECIES>  substrates;
        std::vector<SPECIES>  products;
        std::vector<SPECIES> tracked_concentrations;

        bool is_linear;    // is linear
        Velocity k;        // reaction rate
        Concentration c_eq;
        Unit<std::ratio<0>,std::ratio<3>,std::ratio<0>,std::ratio<-1>> solid_mol_volume;
        //const double K;             // reaction coefficient1
        //const double O;             // reaction coefficient2
    };


    struct ReactionConfig{

        std::vector<SPECIES>  allS;
        std::vector<SPECIES>  solubleS;
        std::vector<SPECIES>  solidS;

    };



    }



#endif //KARST_4_0_REACTIONCONFIG_H
