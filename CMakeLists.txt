cmake_minimum_required(VERSION 3.27)
project(karst_4_0)

set(CMAKE_CXX_STANDARD 20)
include_directories(${CMAKE_SOURCE_DIR}
                    src/external_algorithms
                    src/external_algorithms/triangulation
                    /opt/homebrew/opt/eigen/include/eigen3
)

add_executable(karst_4_0 ./main.cpp
        src/net_elements/GenericElement_impl.h
        src/network/network_topo_generator/network_topo_generator.h
        src/chemistry/LinearKintetics.h
        src/chemistry/GenericKinetics.h)


message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Path to build
set(BUILD_DIRECTORY ./build)

# Tworzenie niestandardowego celu do kopiowania pliku wykonywalnego
add_custom_target(copy_executable ALL
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:karst_4_0> ${BUILD_DIRECTORY}
        DEPENDS karst_4_0
        COMMENT "Copying my_executable to build directory"
)