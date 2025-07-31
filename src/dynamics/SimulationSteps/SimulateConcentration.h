//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_SIMULATECONCENTRATION_H
#define KARST_4_0_SIMULATECONCENTRATION_H

#include <list>
#include "src/dynamics/SimulationSteps/GenericSimulationStep.h"
#include "src/chemistry/ReactionConfig.h"
#include "src/chemistry/LinearKintetics.h"

namespace karst {


    struct SimulateConcentrationState {

        Flow eps_q{0.000001};
        std::map <SPECIES, bool> has_been_set;

    };

    class SimulateConcentrations : public GenericSimulationStep <SimulateConcentrations,SimulateConcentrationState> {

    public:

    protected:

        auto do_init() -> void {
            name = "SimulateConcentrationState";
            for(auto species : S.config.species_to_be_calculated)
                state.has_been_set[species] = false;

    }

        auto do_run()  -> void {

        for(auto species : S.config.species_to_be_calculated)
                state.has_been_set[species] = false;

            for(auto species : S.config.species_to_be_calculated) {
                calculate_species(species);
                state.has_been_set[species] = true;
            }
        }


        auto can_be_calculated(Node& n) const -> bool{  //checked if logic is correct
            return std::ranges::all_of(
                    n.get_nodes_pores(),
                    [&](const NodePore& np) {
                        return (abs(np.p->get_q()) < state.eps_q or
                                np.n->get_u() < n.get_u() or
                                n.is_connected_to_percolation_cluster()
                                );
                    }
            );
        }



        auto outlet_CFlux(NodePore np, SPECIES species) -> CFlux{
            auto [n,p] = np;

            return 0._X;} //FIXME: TO BE IMPLEMENTED


        auto set_new_concentration(Node& n, SPECIES species) -> void{

            Flow Q   = 0._F;
            CFlux QC = 0._X;

            for (auto& [nn,pp]: n.get_nodes_pores())
                if(abs(pp->get_q()) > state.eps_q and nn->get_u() > n.get_u()){
                    Q = Q + abs(pp->get_q());
                    QC = QC + outlet_CFlux(NodePore{.n=nn,.p=pp},species);
                }

            assert(Q>0._F and QC>0._X and "Problem while " + name + " for species"+"for node"<<n.name);
            if(Q>0._F and QC>0._X) n.set_c(species, QC/Q);
        }


        auto calculate_species(SPECIES species) -> void {

            std::cerr<<"Running calculate_species for "<<species<<"..."<<std::endl;
            std::list<Node *> to_be_checked;

            S.apply_to_all_nodes(
                    [](auto& el) {el.clean_percolation_state();}
                    );

            for(auto& n : S.get_inlets()){
                n->set_c(species, S.config.inlet_c.at(species));   //Filling concentration in inlet nodes
                n->set_connected_to_percolation_cluster();
                for (auto& nn : n->get_nodes()){
                    n->set_checked_for_percolation();
                    to_be_checked.push_back(nn);
                    }
            }

            while ( !to_be_checked.empty()){
                bool new_action = false;
                for (auto it = to_be_checked.begin(); it != to_be_checked.end(); ) {

                    if (can_be_calculated(**it)) {
                        new_action = true;
                        set_new_concentration(**it, species);
                        (*it)->set_connected_to_percolation_cluster();
                        for(auto &[nn,pp] : (*it)->get_nodes_pores())
                            if(!nn->is_checked_for_percolation() and pp->get_q()!=0._F){
                                nn->set_checked_for_percolation();
                                to_be_checked.push_back(nn);
                            }
                        it = to_be_checked.erase(it);
                    }
                    else  ++it;
                }

                if(!new_action){
                    state.eps_q=state.eps_q*2;
                    std::cerr<<"Node::epsilon_for_c has been updated: "<<state.eps_q<<std::endl;
                }
            }
    }


    };
} // namespace karst



#endif //KARST_4_0_SIMULATECONCENTRATION_H
