//
// Created by Agnieszka on 31/07/2025.
//

#ifndef KARST_4_0_GENERICKINETICS_H
#define KARST_4_0_GENERICKINETICS_H

#include "src/utils.h"
#include "src/units.h"
#include "src/network/Network.h"
#include "src/dynamics/SimulationConfig.h"
#include "src/dynamics/SimulationState.h"

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


        void prepare_linear_kinetics();

        std::map<SPECIES, std::function<Concentration(NodePore)>> outlet_concentration_map;
        std::map<SPECIES, std::function<Volume(NodePore)>>        delta_volume_map;

    protected:

        ReactionConfig config;
        std::map<REACTIONS,ChemicalReaction> R;
        Network &S;
        const SimulationConfig &sim_config;
        const SimulationState  &sim_state;

    };

}
#endif //KARST_4_0_GENERICKINETICS_H
