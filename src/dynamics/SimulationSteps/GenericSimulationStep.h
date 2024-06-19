//
// Created by Agnieszka on 24/05/2024.
//

#ifndef KARST_4_0_GENERICSIMULATIONSTEP_H
#define KARST_4_0_GENERICSIMULATIONSTEP_H


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
