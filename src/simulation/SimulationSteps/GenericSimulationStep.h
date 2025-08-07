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
        };
        explicit GenericSimulationStep(const Config& c):
                                       S{c.S}, R{c.R}, sim_config{c.sim_config}, sim_state{c.sim_state}
                                       {}


        ~GenericSimulationStep() = default;


        auto init (Long step0 =0, Time t0 = 0._T)  -> void {
            t = t0; step = step0;
            static_cast<Step&>(*this).do_init();
        }

        auto run  (Long step_to_be_calculated)  -> void {

            std::cerr<<"Running "<<name<<"..."<<std::endl;
            if (step >= step_to_be_calculated) return;      //do nth if simulation step has been already calculated for this time step
            static_cast<Step&>(*this).do_run();
            check_and_accept();
            step = step_to_be_calculated;
            t    = t + sim_state.dt;

        }

        auto check_and_accept () -> void{

            std::cerr<<"Checking "<<name<<"..."<<std::endl;
            if(static_cast<Step&>(*this).do_check()) {

                // static_cast<Step&>(*this)->do_accept(); //TODO: Better implement it differenly
                S.apply_to_all_nodes([](auto &el) {
                    el.update_old_state();
                });
            }
        }



        auto get_step () const        -> Long          { return step; }
        auto get_time () const        -> Time          { return t; }


        //Saving info
        auto log_state(const std::ostream& log_file)    ->  void {}


        const SimulationConfig& sim_config;   ///< SimulationConfig


    protected:

        Network&  S;
        const ReactionKinetics& R;
        Time t {0.};            ///< time step the simulation step was run the last time
        Long step {0};          ///< time step the simulation step was run the last time
        const SimulationState& sim_state;
        StepState state;
        std::string name{"Unknown."};
    };


}




#endif //KARST_4_0_GENERICSIMULATIONSTEP_H
