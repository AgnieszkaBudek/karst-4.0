//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_SIMULATEFLOW_H
#define KARST_4_0_SIMULATEFLOW_H


#include "src/simulation//SimulationSteps/GenericSimulationStep.h"

namespace karst {


    struct SimulateFlowState {
        Unitless eps_q = 1.e-5_U;     ///<acceptable error for flow calculations
        Flow     Q_in  {0.};
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
                log.log_with_context<LogLevel::DEBUG_FULL>(p," .q = "+p.get_q());
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
            log.log_state<LogLevel::DEBUG>(*this,std::format("\nIn checking the flow balance: {:2.7f} < {:2.7f}",
                                                             double(abs(state.Q_in-state.Q_out)/abs(state.Q_in+state.Q_out))
                                                             ,double(state.eps_q)));
            bool success = abs(state.Q_in-state.Q_out)/abs(state.Q_in+state.Q_out) < state.eps_q;
            if constexpr (logger_level_min<=LogLevel::DEBUG_PS)
                if (!success) ps_for_debug("\nIn checking the flow.");
            return success;
        }

        auto do_mix_states() -> void {

            for(auto& p :S.get_pores()){
                const auto s_new = p.get_state();
                const auto s_old = p.get_old_state();
                p.set_q((s_old.q+s_new.q)/2.);
            }
        }

        auto do_go_back(){
            log.log_with_context<LogLevel::ERROR>(*this,"Not implemented. Shouldn't be necessary.");
        }

        std::string do_get_state_info() const{
            return
                "\n\t\teps_q = " + state.eps_q +
                "\n\t\tQ_in  = " + state.Q_in +
                "\n\t\tQ_out = " + state.Q_out;
        }

        void do_ps_for_debug(std::string& str) const{

            S.io_mod.print_net_ps_with_values(
                    get_context_info() + " " + str,
                    S.get_nodes(),
                    S.get_pores(),
                    std::vector<Grain>{},
                    [](auto& el){return std::format("{:3.2f}",double(el.get_u()));},
                    [](auto& el){return std::format("{:3.2f}",double(el.get_q()));},
                    [](auto& el){return "";}
            );
        }

    };
} // namespace karst



#endif //KARST_4_0_SIMULATEFLOW_H
