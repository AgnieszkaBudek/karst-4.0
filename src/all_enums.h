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

namespace karst{

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


    enum class SimulationStateType          {WARNING, NORMAL, BREAKTHROUGH};
    // Specialization of EnumToString
    template<>
    const std::map<SimulationStateType, std::string> EnumToString<SimulationStateType>::mapping = {
            {SimulationStateType::WARNING,      "WARNING" },
            {SimulationStateType::NORMAL,       "NORMAL" },
            {SimulationStateType::BREAKTHROUGH, "BREAKTHROUGH" }
    };


    enum class TimeAdaptationMode {INCREASE, DECREASE, NEUTRAL};
    template<>
    const std::map<TimeAdaptationMode, std::string> EnumToString<TimeAdaptationMode>::mapping = {
            {TimeAdaptationMode::INCREASE,  "INCREASE" },
            {TimeAdaptationMode::DECREASE,  "DECREASE" },
            {TimeAdaptationMode::NEUTRAL,   "NEUTRAL" }
    };


    // enum class for different types of topology
    enum class TypeOfNetTopology  {HEXAGONAL, FROM_FILE, FROM_TRIANGULATION};

    // Specialization of EnumToString for TypeOfNetTopology
    template<>
    const std::map<TypeOfNetTopology, std::string> EnumToString<TypeOfNetTopology>::mapping = {
            { TypeOfNetTopology::HEXAGONAL, "HEXAGONAL" },
            { TypeOfNetTopology::FROM_FILE, "FROM_FILE" },
            { TypeOfNetTopology::FROM_TRIANGULATION, "FROM_TRIANGULATION" }
    };


    // enum class for different types of merging
    enum class TypeOfMerging { MERGING_NONE, MERGING_GRAINS };
    // Specialization of EnumToString
    template<>
    const std::map<TypeOfMerging, std::string> EnumToString<TypeOfMerging>::mapping = {
            { TypeOfMerging::MERGING_NONE,   "MERGING_NONE" },
            { TypeOfMerging::MERGING_GRAINS, "MERGING_GRAINS" }
    };


    enum class PrintingMode {DEBUGGING_PRINT_M, GRAIN_PRINT_M};
    // Specialization of EnumToString
    template<>
    const std::map<PrintingMode, std::string> EnumToString<PrintingMode>::mapping = {
            { PrintingMode::DEBUGGING_PRINT_M, "DEBUGGING_PRINT_M" },
            { PrintingMode::GRAIN_PRINT_M, "GRAIN_PRINT_M" }
    };


    enum class StepStateType          {WARNING, NORMAL, ERROR};
    // Specialization of EnumToString
    template<>
    const std::map<StepStateType, std::string> EnumToString<StepStateType>::mapping = {
            {StepStateType::WARNING ,    "WARNING" },
            {StepStateType::NORMAL  ,    "NORMAL"  },
            {StepStateType::ERROR   ,    "ERROR"   }
    };

    enum class MatrixSolver          {MUMPS, HYPRE, EIGEN };
    // Specialization of EnumToString
    template<>
    const std::map<MatrixSolver, std::string> EnumToString<MatrixSolver>::mapping = {
            {MatrixSolver::MUMPS ,    "MUMPS" },
            {MatrixSolver::HYPRE  ,   "HYPRE" },
            {MatrixSolver::EIGEN   ,  "EIGEN" }
    };


    enum class Pressure_Algorithm       {CLASSIC, SMALL_PORES, BIG_PORES};
    // Specialization of EnumToString
    template<>
    const std::map<Pressure_Algorithm, std::string> EnumToString<Pressure_Algorithm>::mapping = {
            {Pressure_Algorithm::CLASSIC,       "CLASSIC" },
            {Pressure_Algorithm::SMALL_PORES,   "SMALL_PORES" },
            {Pressure_Algorithm::BIG_PORES,     "BIG_PORES" },
    };

    enum class PoreGeometry       {CYLINDER, THIN_A, THICK_A, U_SHAPE};
    template<>
    const std::map<PoreGeometry, std::string> EnumToString<PoreGeometry>::mapping = {
            {PoreGeometry::CYLINDER, "CYLINDER" },  // standard cylindrical
            {PoreGeometry::THIN_A,   "THIN_A" },    //thin aperture
            {PoreGeometry::THICK_A,  "THICK_A" },   //thick aperture
            {PoreGeometry::U_SHAPE,  "U_SHAPE" },   //for experiments with Michal
    };

    enum class PoreType       {MATRIX, FRACTURE};
    template<>
    const std::map<PoreType, std::string> EnumToString<PoreType>::mapping = {
            {PoreType::MATRIX,      "MATRIX" },
            {PoreType::FRACTURE,    "FRACTURE" },

    };

}



#endif //KARST_4_0_ALL_ENUMS_H
