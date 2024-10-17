# ifndef KARST_4_0_UTILS_H
# define KARST_4_0_UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <queue>
#include <map>
#include <numeric>
#include <cassert>


#include "src/all_enums.h"

namespace karst {


    inline const double NaN = std::numeric_limits<double>::quiet_NaN();


    class ofstream_ps         : public std::ofstream  {};
    class ofstream_ps_pores   : public ofstream_ps    {};
    class ofstream_ps_grains  : public ofstream_ps    {};   ///< output PostScript file
    class ofstream_txt        : public std::ofstream  {};   ///< output text file
    class log_stream          : public std::ostream   {};   ///< log files



    // Tuple to Struct template...
    template <typename T, typename Tuple>
    T tupleToStruct(Tuple&& tuple) {
        return std::apply([](auto&&... args) {
            return T{std::forward<decltype(args)>(args)...}; }, std::forward<Tuple>(tuple));
    }


//
//    // boll<<str interface //TODO: niepotrzebne, bo mam std::strinstream >> bool, wykorzystać to!!! ale tam problem z potrzebą bool
//    bool& operator <<(bool& b, const std::string& str){
//        if (str == "true")
//            b = true;
//        else if (str == "false")
//            b = false;
//        else
//            throw std::invalid_argument("Unknown string value for bool: " + str);
//
//    }




}

#endif       //KARST_4_0_UTILS_H