//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_REACTION_CONFIG_H
#define KARST_4_0_REACTION_CONFIG_H

#include "src/utils.h"

namespace karst{

     enum class SPECIES{
        A,
        B,
        C,
        E
    };

    template<>
    const std::map<SPECIES, std::string> EnumToString<SPECIES>::mapping = {
            { SPECIES::A, "A" },
            { SPECIES::B, "B" },
            { SPECIES::C, "C" },
            { SPECIES::E, "E" },
    };
//
//    // Operator << specialization for SPECIES  //TODO: wyrzucić potem
//    std::ostream& operator<<(std::ostream& os, SPECIES value) {
//        return os << EnumToString<SPECIES>::mapping.at(value);
//    }


    inline static const std::deque<SPECIES>  solubleS {SPECIES::B, SPECIES::C};
    inline static const std::deque<SPECIES>  solidS   {SPECIES::A, SPECIES::E};

    struct ChemicalReaction{
        const std::deque<SPECIES>  substrates;
        const std::deque<SPECIES>  products;

        const std::deque<SPECIES> tracked_concentrations;

        const bool is_linear;    // is linear
        const Velocity k;        // reaction rate
        const double c_eq;
        //const double K;             // reaction coefficient1
        //const double O;             // reaction coefficient2
    };


    const auto DISSOLUTION = ChemicalReaction {
        .substrates = {SPECIES::A, SPECIES::B},
        .products   = {SPECIES::C},
        .tracked_concentrations = {SPECIES::B},
        .is_linear = true,
        .k = 1._v,
        .c_eq = 0};
        // .K = INFINITY,
        // .O = 0};

    const auto PRECIPITATION = ChemicalReaction {
            .substrates = {SPECIES::C},
            .products   = {SPECIES::E},
            .tracked_concentrations = {SPECIES::C},
            .is_linear = true,
            .k = 1._v,
            .c_eq = 0};
            // .K = INFINITY,
            // .O = 0};

    inline const auto allReactions = std::deque<ChemicalReaction> {DISSOLUTION, PRECIPITATION};

    }



#endif //KARST_4_0_REACTION_CONFIG_H
