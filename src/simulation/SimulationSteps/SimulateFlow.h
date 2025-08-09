//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_SIMULATEFLOW_H
#define KARST_4_0_SIMULATEFLOW_H


#include "src/simulation//SimulationSteps/GenericSimulationStep.h"

namespace karst {


    struct SimulateFlowState {
        Unitless eps_q = 1.e-5_U;     ///<acceptable error for flow calculations
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
                //std::cerr<<"perm = "<<p.get_perm()<<"\t u1 = "<<p.get_nodes()[0]->get_u()<<"\t u2 = "<<p.get_nodes()[1]->get_u()<<std::endl;
                std::cerr<<"q = "<<p.get_q()<<std::endl;
            }
        }

        auto rescale_flow_field_to_keep_Q() -> void{
            Flow Q_tmp {0.};
            for(auto& in : S.get_inlets())
                for(auto p : in->get_pores())
                    Q_tmp = Q_tmp + p->get_q();


            ASSERT_MSG(Q_tmp>=0._F , "ERROR: Problem with inlet flow in calculating flow." + std::to_string(double(Q_tmp)));
            for(auto& p : S.get_pores())
                p.set_q(p.get_q()*S.config.Q_tot/Q_tmp);
        }

        auto do_check()-> bool{
            Flow Q_in {0.};
            Flow Q_out {0.};
            for(auto in : S.get_inlets())
                for(auto p : in->get_pores())
                    Q_in = Q_in + abs(p->get_q());
            for(auto out : S.get_outlets())
                for(auto p : out->get_pores())
                    Q_out = Q_out + abs(p->get_q());

            ASSERT_MSG(Q_in>0._F and Q_out>0._F , "Q_in = "+std::to_string(double(Q_in))+"\t Q_out = "+std::to_string(double(Q_out)));
            return abs(Q_in-Q_out)/abs(Q_in+Q_out) < state.eps_q;
        }



    };
} // namespace karst



#endif //KARST_4_0_SIMULATEFLOW_H
