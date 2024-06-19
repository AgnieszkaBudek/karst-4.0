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

#include "src/units.h"
#include "src/chemistry/reaction_config.h"

namespace karst {

    class Network;
    class Pore;
    class Node;
    class Grain;

    const double NaN = std::numeric_limits<double>::quiet_NaN();


    class ofstream_ps         : public std::ofstream  {};
    class ofstream_ps_pores   : public ofstream_ps    {};
    class ofstream_ps_grains  : public ofstream_ps    {};   ///< output PostScript file
    class ofstream_txt        : public std::ofstream  {};   ///< output text file
    class log_stream          : public std::ostream   {};   ///< log files


    // Enum <-> string interface

    template<typename T>
    struct EnumToString {
        static const std::map<T, std::string> mapping;
    };

    template<typename T>
    typename std::enable_if<std::is_enum<T>::value, std::ostream&>::type
    operator<<(std::ostream& os, T value) {
        const auto& mapping = EnumToString<T>::mapping;
        auto it = mapping.find(value);
        if (it != mapping.end()) {
            os << it->second;
        } else {
            os << "Unknown enum value :/";
        }
        return os;
    }


    template<typename T>
    typename std::enable_if<std::is_enum<T>::value, T&>::type
    operator<<(T& enumVar, const std::string& str) {
        const auto& mapping = EnumToString<T>::mapping;
        auto it = std::find_if(mapping.begin(), mapping.end(), [&str](const auto& pair) {
            return pair.second == str;
        });
        if (it != mapping.end()) {
            enumVar = it->first;
        } else {
            throw std::invalid_argument("Unknown string value for enum: " + str);
        }
        return enumVar;
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