//
// Created by Agnieszka on 31/07/2025.
//

#ifndef KARST_4_0_REACTIONS_KINETIC_H
#define KARST_4_0_REACTIONS_KINETIC_H

#include<src/network/Network.h>
#include<src/dynamics/Simulation.h>
#include<src/chemistry/reaction_config.h>

namespace karst{

    class LinearKinetics {
        public:

            Network& N;
            Simulation& S;

            auto B_outlet(NodePore np) -> Concentration {
                auto [n,p] = np;
                Unitless      da            = p->get_surface()*DISSOLUTION.k/p->get_q();                                ///< Damköhler  for dissolution
                Concentration drop_tmp      = n->get_c(SPECIES::B)*(1.-exp(-double(da)));                               ///< default concentration drop in pore *p
                Volume        delta_A_tmp   = drop_tmp * S.get_state().dt * p->get_q()*DISSOLUTION.solid_mol_volume;    ///< default A consumption
                Volume        max_delta_A   = p->get_available_volume(SPECIES::A);                                      ///< total amount of A that is available

                if(delta_A_tmp <= max_delta_A)
                    return n->get_c(SPECIES::B)*exp(-double(da));
                else
                    return n->get_c(SPECIES::B) - delta_A_tmp/(S.get_state().dt*p->get_q()*DISSOLUTION.solid_mol_volume);
            }


            auto delta_A(NodePore &np) -> Volume {
                auto [n, p] = np;
                Concentration drop_c = abs(p->get_nodes()[0]->get_c(SPECIES::B) - p->get_nodes()[1]->get_c(SPECIES::B));
                return -S.get_state().dt * p->get_q() * drop_c * DISSOLUTION.solid_mol_volume;
            }

            auto C_outlet(NodePore np) -> Concentration {

                auto [n,p] = np;
                Unitless      da1           = p->get_surface() * DISSOLUTION.k   / p->get_q();
                Unitless      da2           = p->get_surface() * PRECIPITATION.k / p->get_q();

                Concentration c_prod = n->get_c(SPECIES::B)*(exp(-double(da1)) - exp(-double(da2))*(double(da1)))/(da2-da1);
                Concentration c_cons = n->get_c(SPECIES::C)*(1-exp(double(da2)));

                return c_prod + c_cons; //FIXME: add
            }


            auto delta_E(NodePore &np) -> Volume {
                auto [n, p] = np;
                Concentration drop_c = abs(p->get_nodes()[0]->get_c(SPECIES::C) - p->get_nodes()[1]->get_c(SPECIES::C));
                return -S.get_state().dt * p->get_q() * drop_c * PRECIPITATION.solid_mol_volume;
            }

    };
};

#endif //KARST_4_0_REACTIONS_KINETIC_H
