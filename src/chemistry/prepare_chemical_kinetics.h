//
// Created by Agnieszka on 31/07/2025.
//

#ifndef KARST_4_0_PREPARE_CHEMICAL_KINETICS_H
#define KARST_4_0_PREPARE_CHEMICAL_KINETICS_H

#include<src/network/Network.h>
#include<src/simulation/SimulationState.h>
#include<src/simulation/SimulationConfig.h>
#include<src/chemistry/ReactionKinetics.h>

namespace karst {


    inline auto ReactionKinetics::prepare_linear_dissolution() -> void {

        solubleS = {SOLUBLES::B};
        solidS   = {SOLIDS::A};

        R[REACTION::D] = ChemicalReaction {
                .name = "dissolution",
                .tracked_solids   = {SOLIDS::A},
                .tracked_solubles = {SOLUBLES::B},
                .k {0.1},
                .c_eq {0.},
                .solid_mol_volume {1.}
        };


        outlet_concentration_map [SOLUBLES::B] =  [&](NodePore np) {
            auto [n, p] = np;

            if (!(p->get_q() > 0._F)) return n->get_c(SOLUBLES::B);

            Volume max_delta_A = p->get_available_volume(SOLIDS::A);
            if (max_delta_A == 0._V ) return n->get_c(SOLUBLES::B);

            Unitless da = p->get_surface_tot() * R[REACTION::D].k / p->get_q();
            Concentration drop_tmp = n->get_c(SOLUBLES::B) * (1. - exp(-double(da)));
            Volume delta_A_tmp = drop_tmp * sim_state.dt * p->get_q() * R[REACTION::D].solid_mol_volume;

            if (delta_A_tmp <= max_delta_A)
                return n->get_c(SOLUBLES::B) * exp(-double(da));
            else
                return n->get_c(SOLUBLES::B) - delta_A_tmp / (sim_state.dt * p->get_q() * R[REACTION::D].solid_mol_volume);
        };


        delta_volume_map[SOLIDS::A] = [&](Pore& p) {
            if (!(p.get_q() > 0._F)) return 0._V;
            Concentration drop_c = abs(p.get_nodes()[0]->get_c(SOLUBLES::B) - p.get_nodes()[1]->get_c(SOLUBLES::B));
            return -sim_state.dt * p.get_q() * drop_c * R[REACTION::D].solid_mol_volume;
        };



        // TODO: calculate here k from Da or the other way round !!!
    }


    inline auto ReactionKinetics::prepare_linear_dissolution_and_precipitation() -> void {

        solubleS = {SOLUBLES::B, SOLUBLES::C};
        solidS   = {SOLIDS::A,  SOLIDS::E};

        R[REACTION::D] = ChemicalReaction {
                .name = "dissolution",
                .tracked_solids   = {SOLIDS::A},
                .tracked_solubles = {SOLUBLES::B},
                .k {1.},
                .c_eq {0.},
                .solid_mol_volume {1.}
        };

        R[REACTION::P] = ChemicalReaction {
                .name = "precipitation",
                .tracked_solids   = {SOLIDS::E},
                .tracked_solubles = {SOLUBLES::C},
                .k {1.},
                .c_eq{0.},
                .solid_mol_volume{1.}
        };

        outlet_concentration_map [SOLUBLES::B] =  [&](NodePore np) {
            auto [n, p] = np;
            if (!(p->get_q() > 0._F)) return n->get_c(SOLUBLES::B);
            Unitless da = p->get_surface_tot() * R[REACTION::D].k / p->get_q();
            Concentration drop_tmp = n->get_c(SOLUBLES::B) * (1. - exp(-double(da)));
            Volume delta_A_tmp = drop_tmp * sim_state.dt * p->get_q() * R[REACTION::D].solid_mol_volume;
            Volume max_delta_A = p->get_available_volume(SOLIDS::A);
            if (delta_A_tmp <= max_delta_A)
                return n->get_c(SOLUBLES::B) * exp(-double(da));
            else
                return n->get_c(SOLUBLES::B) - delta_A_tmp / (sim_state.dt * p->get_q() * R[REACTION::D].solid_mol_volume);
        };


        delta_volume_map[SOLIDS::A] = [&](Pore& p) {
            if (!(p.get_q() > 0._F)) return 0._V;
            Concentration drop_c = abs(p.get_nodes()[0]->get_c(SOLUBLES::B) - p.get_nodes()[1]->get_c(SOLUBLES::B));
            return -sim_state.dt * p.get_q() * drop_c * R[REACTION::D].solid_mol_volume;
        };



        // TODO: calculate here k from Da or the other way round !!!
    }

}
#endif //KARST_4_0_PREPARE_CHEMICAL_KINETICS_H


//        auto C_outlet(NodePore np) -> Concentration {
//
//            auto [n, p] = np;
//
//            if (!(p->get_q() > 0._F)) return n->get_c(SPECIES::C);
//
//            Unitless da1 = p->get_surface() * DISSOLUTION.k / p->get_q();
//            Unitless da2 = p->get_surface() * PRECIPITATION.k / p->get_q();
//
//            Concentration c_prod =
//                    n->get_c(SPECIES::B) * (exp(-double(da1)) - exp(-double(da2)) * (double(da1))) / (da2 - da1);
//            Concentration c_cons = n->get_c(SPECIES::C) * (1 - exp(double(da2)));
//
//            return c_prod + c_cons; //FIXME: add all corner cases!!!
//        }
//
//
//        auto delta_E(NodePore &np) -> Volume {
//            auto [n, p] = np;
//            if (!(p->get_q() > 0._F)) return 0._V;
//
//            Unitless da1 = p->get_surface() * DISSOLUTION.k / p->get_q();
//            Unitless da2 = p->get_surface() * PRECIPITATION.k / p->get_q();
//            Concentration c_prod =
//                    n->get_c(SPECIES::B) * (exp(-double(da1)) - exp(-double(da2)) * (double(da1))) / (da2 - da1);
//            Concentration c_cons = n->get_c(SPECIES::C) * (1 - exp(double(da2)));
//
//            return 0._V;  ///< TODO: implement it!
//        }
