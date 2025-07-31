//
// Created by Agnieszka on 31/07/2025.
//

#ifndef KARST_4_0_LINEARKINTETICS_H
#define KARST_4_0_LINEARKINTETICS_H

#include<src/network/Network.h>
#include<src/dynamics/SimulationState.h>
#include<src/dynamics/SimulationConfig.h>
#include<src/chemistry/GenericKinetics.h>

namespace karst {



        inline auto ReactionKinetics::prepare_linear_kinetics() -> void {

            config.solubleS = {SPECIES::B, SPECIES::C};
            config.solidS = {SPECIES::A, SPECIES::E};

            R.emplace(REACTIONS::D, ChemicalReaction {
                    .name = "dissolution",
                    .substrates = {SPECIES::A, SPECIES::B},
                    .products   = {SPECIES::C},
                    .tracked_concentrations = {SPECIES::B},
                    .is_linear = true,
                    .k {1.},
                    .c_eq {0.},
                    .solid_mol_volume {1.}
            });

            R.emplace(REACTIONS::P, ChemicalReaction {
                    .name = "precipitation",
                    .substrates = {SPECIES::C},
                    .products   = {SPECIES::E},
                    .tracked_concentrations = {SPECIES::C},
                    .is_linear = true,
                    .k {1.},
                    .c_eq{0.},
                    .solid_mol_volume{1.}
            });

            outlet_concentration_map.emplace(SPECIES::B, [&](NodePore np) {
                auto [n, p] = np;
                if (!(p->get_q() > 0._F)) return n->get_c(SPECIES::B);
                Unitless da = p->get_surface() * R[REACTIONS::D].k / p->get_q();
                Concentration drop_tmp = n->get_c(SPECIES::B) * (1. - exp(-double(da)));
                Volume delta_A_tmp = drop_tmp * sim_state.dt * p->get_q() * R[REACTIONS::D].solid_mol_volume;
                Volume max_delta_A = p->get_available_volume(SPECIES::A);
                if (delta_A_tmp <= max_delta_A)
                    return n->get_c(SPECIES::B) * exp(-double(da));
                else
                    return n->get_c(SPECIES::B) - delta_A_tmp / (sim_state.dt * p->get_q() * R[REACTIONS::D].solid_mol_volume);
            });


            delta_volume_map.emplace(SPECIES::A, [&](NodePore np) {
                auto [n, p] = np;
                if (!(p->get_q() > 0._F)) return 0._V;
                Concentration drop_c = abs(p->get_nodes()[0]->get_c(SPECIES::B) - p->get_nodes()[1]->get_c(SPECIES::B));
                return -sim_state.dt * p->get_q() * drop_c * R[REACTIONS::D].solid_mol_volume;
            });



            // TODO: calculate here k from Da or the other way round !!!
        }
    };

}
#endif //KARST_4_0_LINEARKINTETICS_H


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
