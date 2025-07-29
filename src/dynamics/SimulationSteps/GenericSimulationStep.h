//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_GENERICSIMULATIONSTEP_H
#define KARST_4_0_GENERICSIMULATIONSTEP_H

#include "src/utils.h"
#include "src/units.h"
#include "src/network/Network.h"
#include "src/dynamics/SimulationConfig.h"


namespace karst{
    template < typename Step>
    class GenericSimulationStep
    {
    public:

        explicit GenericSimulationStep(const SimulationConfig& sim_config0) :
                sim_config{sim_config0}
        {}


        ~GenericSimulationStep() {}


        auto init ()  -> void { static_cast<Step&>(*this)->do_init();}
        auto run  ()  -> void { static_cast<Step&>(*this)->do_run(); }


        auto get_time_step () const        -> long          { return step; }


        //Saving info
        auto log_state(const std::ostream& log_file)    ->  void {}


        const SimulationConfig& sim_config;   ///< SimulationConfig

    protected:

        Network&  S;
        Long step{0};                  ///< time step the simulation step was run the last time
        StepStateType state{StepStateType::NORMAL};
    };




}

//auto update(const BType& bar) -> void  //TODO: use this...
//{
//    assert(is_ok(bar));
//
//    auto t = std::chrono::high_resolution_clock::now();
//
//    if constexpr (DebugLevel::check_signals)
//    {
//        check_input_bar(bar);
//        check_m_value("before update");
//    }
//
//    static_cast<Signal&>(*this).do_update(bar);
//
//    if constexpr (DebugLevel::check_signals)
//    {
//        check_m_value("after update");
//    }
//    if(DebugLevel::check_time) check_calculation_time(std::chrono::high_resolution_clock::now()-t);
//}



#endif //KARST_4_0_GENERICSIMULATIONSTEP_H
