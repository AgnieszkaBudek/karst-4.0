//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_GENERICSIMULATIONSTEP_H
#define KARST_4_0_GENERICSIMULATIONSTEP_H

#include "src/utils.h"
#include "src/units.h"
#include "src/network/Network.h"
#include "src/simulation/SimulationConfig.h"
#include "src/simulation/SimulationState.h"
#include "src/chemistry/ReactionKinetics.h"


namespace karst{
    template <typename Step, typename StepState>
    class GenericSimulationStep
    {
    public:
        struct Config
        {
            Network& S;
            const ReactionKinetics& R;
            const SimulationConfig& sim_config;
            const SimulationState& sim_state;
            Logger<logger_level_min>& log;
        };
        explicit GenericSimulationStep(const Config& c):
                                       S{c.S},R{c.R}, sim_config{c.sim_config}, sim_state{c.sim_state},log{c.log}
                                       {}


        ~GenericSimulationStep() = default;


        auto init (Long step0 =0, Time t0 = 0._T)  -> void {
            t = t0; step = step0;
            static_cast<Step&>(*this).do_init();
        }

        auto run  (Long step_to_be_calculated, Time t_to_be_calculate)  -> void {

//            if (step >= step_to_be_calculated or t >= t_to_be_calculate){
//                log.log_with_context<LogLevel::INFO>( *this," nth to be done...");
//                return;      //do nth if simulation step has been already calculated for this time step
//            }

            log.log_with_context<LogLevel::INFO>( *this," is running...");
            static_cast<Step&>(*this).do_run();
            log.log_with_context<LogLevel::INFO>( *this," is being checked...");
            bool success = static_cast<Step&>(*this).do_check();
            ASSERT_MSG(success,"Problem with " + get_context_info());

            log.log_state<LogLevel::DEBUG>(static_cast<Step&>(*this),"");

            step = step_to_be_calculated;
            t    = t_to_be_calculate;

        }



        auto update_old_state() -> void{
            log.log_with_context<LogLevel::INFO>(*this,"Updating old_state...");
            S.apply_to_all_net_elements([](auto &el) {
                el.update_old_state();
            });
        };

        auto mix_states(){
            log.log_with_context<LogLevel::INFO>(*this,"Mixing state_old and state_new...");
            static_cast<Step&>(*this).do_mix_states();
        }

        auto go_back(){
            log.log_with_context<LogLevel::INFO>(*this,"Reverting to state_old...");
            static_cast<Step&>(*this).do_go_back();
        }


        auto get_step () const        -> Long          { return step; }
        auto get_time () const        -> Time          { return t; }

        //Saving info
        std::string get_context_info() const {
            return  name + " [" + std::to_string(step)+"] ";
        }

        //Saving info
        std::string get_state_info() const {
            return  static_cast<const Step &>(*this).do_get_state_info();
        }


        const SimulationConfig& sim_config;   ///< SimulationConfig


    protected:

        Network&  S;
        const ReactionKinetics& R;
        Time t {-1};            ///< time step the simulation step was run the last time
        Long step {-1};          ///< time step the simulation step was run the last time
        const SimulationState& sim_state;
        StepState state;
        std::string name{"Unknown."};
        Logger<logger_level_min>& log;
    };


}




#endif //KARST_4_0_GENERICSIMULATIONSTEP_H
