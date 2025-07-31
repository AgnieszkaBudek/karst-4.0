//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_SIMULATEPRESSURE_H
#define KARST_4_0_SIMULATEFLOW_H


#include "src/dynamics/SimulationSteps/GenericSimulationStep.h"
#include "external_algorithms/algorithms_cc.h"

namespace karst {


    struct SimulateFlowState {
        Unitless eps_q = 1.e-5_U;     ///<acceptable error for flow calculations
    };

    class SimulateFLow : public GenericSimulationStep <SimulateFLow,SimulateFlowState> {


    protected:

        auto do_init() -> void {name = "SimulateFlow";}

        auto do_run()  -> void {
            calculate_flow_field();
            if(S.config.Q_tot>0._F)
                rescale_flow_field_to_keep_Q();
        }


        auto calculate_flow_field()     -> void{
            for(auto& p : S.get_pores())
                p.set_q(p.get_perm()*(p.get_nodes()[0]->get_u() - p.get_nodes()[1]->get_u()));
        }

        auto rescale_flow_field_to_keep_Q() -> void{
            Flow Q_tmp {0.};
            for(auto& in : S.get_inlets())
                for(auto p : in->get_pores())
                    Q_tmp = Q_tmp + p->get_q();

            assert(Q_tmp>0 and "ERROR: Problem with inlet flow in " + state.name);
            for(auto& p : S.get_pores())
                p.set_q(p.get_q()*S.config.Q_tot/Q_tmp);
        }

        auto do_check()-> bool{
            Flow Q_in {0.};
            Flow Q_out {0.};
            for(auto& in : S.get_inlets())
                for(auto p : in->get_pores())
                    Q_in = Q_in + abs(p->get_q());
            for(auto& out : S.get_outlets())
                for(auto p : out->get_pores())
                    Q_in = Q_in + abs(p->get_q());

            assert(Q_in>0 and Q_out>0 and "ERROR: Problem with inlet/outlet flow in " + state.name);
            return abs(Q_in-Q_out)/abs(Q_in+Q_out) < state.eps_q;
        }



    };
} // namespace karst



#endif //KARST_4_0_SIMULATEPRESSURE_H
