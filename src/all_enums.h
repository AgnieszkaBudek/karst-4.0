//
// Created by Agnieszka on 19/06/2024.
//

#ifndef KARST_4_0_ALL_ENUMS_H
#define KARST_4_0_ALL_ENUMS_H
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
#include <cstddef>
#include <type_traits>

namespace karst{


    template<typename T>
    concept EnumType = std::is_enum_v<T>;

    template<EnumType T>
    constexpr std::size_t enum_size_v = static_cast<std::size_t>(T::SIZE);

    // Enum <-> string interface

    template<EnumType T>
    struct EnumToString {
        static const std::map<T, std::string> mapping;
    };

    template<EnumType T>
    std::ostream& operator<<(std::ostream& os, T value) {
        const auto& mapping = EnumToString<T>::mapping;
        auto it = mapping.find(value);
        if (it != mapping.end()) {
            os << it->second;
        } else {
            os << "Unknown enum value :/";
        }
        return os;
    }


// operator+
    template<EnumType T>
    std::string operator+(const std::string& str, T value) {
        const auto& mapping = EnumToString<T>::mapping;
        auto it = mapping.find(value);
        if (it != mapping.end()) {
            return str + it->second;
        } else {
            std::cerr << "[ERROR] Unknown enum value.\n";
            return str + "UNKNOWN_ENUM";
        }
    }
    // operator+
    template<EnumType T>
    std::string operator+( T value, const std::string& str) {
        const auto& mapping = EnumToString<T>::mapping;
        auto it = mapping.find(value);
        if (it != mapping.end()) {
            return str + it->second;
        } else {
            std::cerr << "[ERROR] Unknown enum value.\n";
            return str + "UNKNOWN_ENUM";
        }
    }


    template<EnumType T>
    std::string operator+(const char* lhs, T rhs) {
        return std::string(lhs) + rhs;
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
            throw std::invalid_argument("[ERROR] Unknown string value for enum: " + str);
        }
        return enumVar;
    }


    enum class SimulationStateType          {WARNING, NORMAL, BREAKTHROUGH, FINISHED, SIZE};
    // Specialization of EnumToString
    template<>
    const std::map<SimulationStateType, std::string> EnumToString<SimulationStateType>::mapping = {
            {SimulationStateType::WARNING,      "WARNING" },
            {SimulationStateType::NORMAL,       "NORMAL" },
            {SimulationStateType::BREAKTHROUGH, "BREAKTHROUGH" },
            {SimulationStateType::FINISHED,     "FINISHED" }
    };


    enum class TimeAdaptationMode {INCREASE, DECREASE, NEUTRAL, SIZE};
    template<>
    const std::map<TimeAdaptationMode, std::string> EnumToString<TimeAdaptationMode>::mapping = {
            {TimeAdaptationMode::INCREASE,  "INCREASE" },
            {TimeAdaptationMode::DECREASE,  "DECREASE" },
            {TimeAdaptationMode::NEUTRAL,   "NEUTRAL" }
    };


    // enum class for different types of topology
    enum class TypeOfNetTopology  {HEXAGONAL, CUBIC, FROM_FILE, FROM_H_FILE, FROM_TRIANGULATION, SIZE};

    // Specialization of EnumToString for TypeOfNetTopology
    template<>
    const std::map<TypeOfNetTopology, std::string> EnumToString<TypeOfNetTopology>::mapping = {
            { TypeOfNetTopology::HEXAGONAL, "HEXAGONAL" },
            { TypeOfNetTopology::CUBIC, "CUBIC" },
            { TypeOfNetTopology::FROM_FILE, "FROM_FILE" },
            { TypeOfNetTopology::FROM_H_FILE, "FROM_H_FILE" },
            { TypeOfNetTopology::FROM_TRIANGULATION, "FROM_TRIANGULATION" }
    };


    // enum class for different types of merging
    enum class TypeOfMerging { MERGING_NONE, MERGING_GRAINS, SIZE};
    // Specialization of EnumToString
    template<>
    const std::map<TypeOfMerging, std::string> EnumToString<TypeOfMerging>::mapping = {
            { TypeOfMerging::MERGING_NONE,   "MERGING_NONE" },
            { TypeOfMerging::MERGING_GRAINS, "MERGING_GRAINS" }
    };


    enum class PrintingMode {DEBUGGING_PRINT_M, GRAIN_PRINT_M, SIZE};
    // Specialization of EnumToString
    template<>
    const std::map<PrintingMode, std::string> EnumToString<PrintingMode>::mapping = {
            { PrintingMode::DEBUGGING_PRINT_M, "DEBUGGING_PRINT_M" },
            { PrintingMode::GRAIN_PRINT_M, "GRAIN_PRINT_M" }
    };


    enum class StepStateType          {WARNING, NORMAL, ERROR, SIZE};
    // Specialization of EnumToString
    template<>
    const std::map<StepStateType, std::string> EnumToString<StepStateType>::mapping = {
            {StepStateType::WARNING ,    "WARNING" },
            {StepStateType::NORMAL  ,    "NORMAL"  },
            {StepStateType::ERROR   ,    "ERROR"   }
    };

    enum class MatrixSolver          {MUMPS, HYPRE, EIGEN, HYPRE_MUMPS, SIZE};
    // Specialization of EnumToString
    template<>
    const std::map<MatrixSolver, std::string> EnumToString<MatrixSolver>::mapping = {
            {MatrixSolver::MUMPS        ,  "MUMPS"       },         //only MUMPS
            {MatrixSolver::HYPRE        ,  "HYPRE"       },         //only HYPRE
            {MatrixSolver::HYPRE_MUMPS  ,  "HYPRE_MUMPS" },         //direct: MUMPS, iterative: HYPRE
            {MatrixSolver::EIGEN        ,  "EIGEN"       }          //direct: EIGEN:llc, iterative: EIGEN::cg
    };


    enum class Pressure_Algorithm       {CLASSIC, SMALL_PORES, BIG_PORES, SIZE};
    // Specialization of EnumToString
    template<>
    const std::map<Pressure_Algorithm, std::string> EnumToString<Pressure_Algorithm>::mapping = {
            {Pressure_Algorithm::CLASSIC,       "CLASSIC" },
            {Pressure_Algorithm::SMALL_PORES,   "SMALL_PORES" },
            {Pressure_Algorithm::BIG_PORES,     "BIG_PORES" },
    };

    enum class PoreGeometry       {CYLINDER, THIN_A, THICK_A, U_SHAPE, THROAT, SIZE};
    template<>
    const std::map<PoreGeometry, std::string> EnumToString<PoreGeometry>::mapping = {
            {PoreGeometry::CYLINDER, "CYLINDER" },   // standard cylindrical
            {PoreGeometry::THIN_A,   "THIN_A"  },    // thin aperture
            {PoreGeometry::THICK_A,  "THICK_A" },    // thick aperture
            {PoreGeometry::U_SHAPE,  "U_SHAPE" },    // for experiments with Michal
            {PoreGeometry::THROAT,   "THROAT"  }     // for throat representation
    };

    enum class PoreType       {MATRIX, FRACTURE, SIZE};
    template<>
    const std::map<PoreType, std::string> EnumToString<PoreType>::mapping = {
            {PoreType::MATRIX,      "MATRIX" },
            {PoreType::FRACTURE,    "FRACTURE" },

    };

    enum class ReactionSet      {LINEAR_DP, LINEAR_D, SIZE};

}



#endif //KARST_4_0_ALL_ENUMS_H
