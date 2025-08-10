//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_SIMULATEFLOW_H
#define KARST_4_0_SIMULATEFLOW_H


#include "src/simulation//SimulationSteps/GenericSimulationStep.h"

namespace karst {


    struct SimulateFlowState {
        Unitless eps_q = 1.e-5_U;     ///<acceptable error for flow calculations
        Flow     Q_in {0.};
        Flow     Q_out {0.};
    };

    class SimulateFlow : public GenericSimulationStep <SimulateFlow,SimulateFlowState> {

    public:
        friend class GenericSimulationStep<SimulateFlow, SimulateFlowState>;
        using GenericSimulationStep::GenericSimulationStep;

    protected:

        auto do_init() -> void {name = "SimulateFlow";}

        auto do_run()  -> void {
            calculate_flow_field();
            if(S.config.Q_tot>0._F)
                rescale_flow_field_to_keep_Q();
        }


        auto calculate_flow_field()     -> void{
            for(auto& p : S.get_pores()){
                p.set_q(p.get_perm()*(p.get_nodes()[0]->get_u() - p.get_nodes()[1]->get_u()));
                std::cerr<<"q = "<<p.get_q()<<std::endl;
            }
        }

        auto rescale_flow_field_to_keep_Q() -> void{
            Flow Q_tmp {0.};
            for(auto& in : S.get_inlets())
                for(auto p : in->get_pores())
                    Q_tmp = Q_tmp + p->get_q();


            ASSERT_MSG(Q_tmp>=0._F , "Problem with inlet flow in calculating flow." + std::to_string(double(Q_tmp)));
            for(auto& p : S.get_pores())
                p.set_q(p.get_q()*S.config.Q_tot/Q_tmp);
        }

        auto do_check()-> bool{
            state.Q_in  = 0._F;
            state.Q_out = 0._F;
            for(auto in : S.get_inlets())
                for(auto p : in->get_pores())
                    state.Q_in += abs(p->get_q());
            for(auto out : S.get_outlets())
                for(auto p : out->get_pores())
                    state.Q_out += abs(p->get_q());

            ASSERT_MSG(state.Q_in>0._F and state.Q_out>0._F , "Q_in = "+state.Q_in+"\t Q_out = "+state.Q_out);
            return abs(state.Q_in-state.Q_out)/abs(state.Q_in+state.Q_out) < state.eps_q;
        }

        std::string do_get_state_info() const{
            return "\t.eps_q = "+state.eps_q + "\t.Q_in" +state.Q_in + "\t.Q_out" + state.Q_out;
        }

    };
} // namespace karst



#endif //KARST_4_0_SIMULATEFLOW_H
