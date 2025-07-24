//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_REACTION_CONFIG_H
#define KARST_4_0_REACTION_CONFIG_H

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

    inline static const std::vector<SPECIES>  solubleS {SPECIES::B, SPECIES::C};
    inline static const std::vector<SPECIES>  solidS   {SPECIES::A, SPECIES::E};

    struct ChemicalReaction{
        const std::vector<SPECIES>  substrates;
        const std::vector<SPECIES>  products;

        const std::vector<SPECIES> tracked_concentrations;

        const bool is_linear;    // is linear
        const Velocity k;        // reaction rate
        const double c_eq;
        //const double K;             // reaction coefficient1
        //const double O;             // reaction coefficient2
    };


    inline const auto DISSOLUTION = ChemicalReaction {
        .substrates = {SPECIES::A, SPECIES::B},
        .products   = {SPECIES::C},
        .tracked_concentrations = {SPECIES::B},
        .is_linear = true,
        .k = 1._v,
        .c_eq = 0};
        // .K = INFINITY,
        // .O = 0};

    inline const auto PRECIPITATION = ChemicalReaction {
            .substrates = {SPECIES::C},
            .products   = {SPECIES::E},
            .tracked_concentrations = {SPECIES::C},
            .is_linear = true,
            .k = 1._v,
            .c_eq = 0};
            // .K = INFINITY,
            // .O = 0};

    inline const auto allReactions = std::vector<ChemicalReaction> {DISSOLUTION, PRECIPITATION};

    }



#endif //KARST_4_0_REACTION_CONFIG_H
