cmake_minimum_required(VERSION 3.27)
project(karst_4_0)

set(CMAKE_CXX_STANDARD 20)
include_directories(${CMAKE_SOURCE_DIR}
                    src/external_algorithms
                    src/external_algorithms/triangulation
)

add_executable(karst_4_0 main.cpp
        src/utils.h
        src/units.h
        src/net_elements/GenericElement.h
        src/net_elements/Pore.h
        src/net_elements/Grain.h
        src/network/Network.h
        src/net_elements/Node.h
        src/network/NetworkConfig.h
        src/chemistry/reaction_config.h
        src/network/NetworkState.h
        src/network/NetworkTopologyConfig.h
        src/dynamics/SimulationConfig.h
        src/dynamics/Simulation.h
        src/dynamics/SimulationState.h
        src/import_export/PrintingConfig.h
        src/read_config.h
        src/analyzing/AnalizePatternConfig.h
        src/analyzing/AnalizePattern.h
        src/dynamics/SimulationSteps/SimulateFlow.h
        src/dynamics/SimulationSteps/SimulateConcentration.h
        src/dynamics/SimulationSteps/SimulateSizes.h
        src/dynamics/SimulationSteps/SimulateMerging.h
        src/dynamics/SimulationSteps/GenericSimulationStep.h
        src/dynamics/EulerStep.h
        src/dynamics/LeapFrogStep.h
        src/import_export/PrintingModule.h
        src/import_export/read_net_from_files.h
        src/analyzing/percolation.h
        src/network/network_topo_generators.h
        src/import_export/printing_ps.h
        src/import_export/printing_primitives.h
        src/all_enums.h
        src/import_export/printing_net_elements.h
        src/external_algorithms/triangulation/triangulation.h

)


message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Path to build
set(BUILD_DIRECTORY ./build)

# Tworzenie niestandardowego celu do kopiowania pliku wykonywalnego
add_custom_target(copy_executable ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:karst_4_0> ${BUILD_DIRECTORY}
        DEPENDS karst_4_0
        COMMENT "Copying my_executable to build directory"
)