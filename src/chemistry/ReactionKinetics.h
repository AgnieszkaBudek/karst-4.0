//
// Created by Agnieszka on 31/07/2025.
//

#ifndef KARST_4_0_REACTIONKINETICS_H
#define KARST_4_0_REACTIONKINETICS_H

#include "src/utils.h"
#include "src/units.h"
#include "src/network/Network.h"
#include "src/simulation/SimulationConfig.h"
#include "src/simulation/SimulationState.h"

namespace karst {

    class ReactionKinetics {
    public:


        explicit ReactionKinetics(Network &S0,
                                 const SimulationConfig &sim_config0,
                                 const SimulationState &sim_state0) :
                S{S0},
                sim_config{sim_config0},
                sim_state{sim_state0} {}


        ~ReactionKinetics() = default;

        //interface
        auto get_solubleS() const -> std::vector<SOLUBLES> {return solubleS;};
        auto get_solidS()   const -> std::vector<SOLIDS>   {return solidS;};

        auto get_outlet_concentration(SOLUBLES sp)   const -> std::function<Concentration(NodePore)>{
            assert(outlet_concentration_map.contains(sp) and "In outlet_concentration_map: "+sp+" does not belong to outlet_concentration_map. ");
            return outlet_concentration_map[sp];}

        auto get_delta_volume_map(SOLIDS sp)   const -> std::function<Volume(Pore&)>{
            assert(delta_volume_map.contains(sp) and "In get_delta_volume_map: "+sp+" does not belong to delta_volume_map. ");
            return delta_volume_map[sp];}

        auto get_reaction(REACTION r)   const -> ChemicalReaction{
            assert(R.contains(r) and "In Reactions: "+r+" does not belong to reactions. ");
            return R[r];}

        //preparation for given reactions
        void prepare_linear_kinetics();

        auto check_implementation() -> bool{

            return(
            std::ranges::all_of(solubleS, [&](const auto& sp) {
                return outlet_concentration_map[sp] != nullptr;})
                and
            std::ranges::all_of(solidS, [&](const auto& sp) {
                return delta_volume_map[sp] != nullptr ;})
            );
        }


    protected:

        std::vector<SOLUBLES>  solubleS;
        std::vector<SOLIDS>    solidS;
        EnumArray <REACTION, ChemicalReaction,                        enum_size_v<REACTION>> R {ChemicalReaction{}};
        EnumArray <SOLUBLES, std::function<Concentration(NodePore)>,  enum_size_v<SOLUBLES>> outlet_concentration_map{nullptr};
        EnumArray <SOLIDS,   std::function<Volume(Pore&)>,            enum_size_v<SOLIDS>>   delta_volume_map{nullptr};
        Network &S;
        const SimulationConfig &sim_config;
        const SimulationState  &sim_state;

    };

}
#endif //KARST_4_0_REACTIONKINETICS_H
