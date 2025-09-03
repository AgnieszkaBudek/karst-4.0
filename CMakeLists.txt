cmake_minimum_required(VERSION 3.27)
project(karst_4_0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Build configuration ---
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build - enabling debug symbols")
    add_compile_options(-g -O0 -ggdb)

    # Sprawdź czy ASAN jest dostępny (działa tylko z Clang na macOS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT CMAKE_SYSTEM_NAME MATCHES "Linux")
        message(STATUS "Enabling AddressSanitizer for Clang")
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    else()
        message(STATUS "AddressSanitizer not available for GCC on macOS")
    endif()

else()
    message(STATUS "Release build - enabling optimizations")
    add_compile_options(-O3 -DNDEBUG)
endif()

# --- VTK configuration ---
set(VTK_DIR "/opt/homebrew/lib/cmake/vtk-9.4")
message(STATUS "Using VTK from: ${VTK_DIR}")

find_package(VTK REQUIRED COMPONENTS
        CommonCore
        CommonDataModel
        IOXML
)

if(NOT VTK_FOUND)
    message(FATAL_ERROR "VTK not found! Check installation.")
endif()

message(STATUS "VTK version: ${VTK_VERSION}")

find_package(Eigen3 REQUIRED)
find_package(Boost 1.89 REQUIRED COMPONENTS serialization)

# --- Include directories ---
include_directories(
        ${CMAKE_SOURCE_DIR}
        src
        src/external_algorithms
        src/external_algorithms/triangulation
        ${EIGEN3_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${VTK_INCLUDE_DIRS}
)

# --- Executable ---
add_executable(karst_4_0 main.cpp
#        src/import_export/VTK_by_heand.h
#        src/import_export/read_from_csv.h
#        src/external_algorithms/CsvParser/csv.h
)

# --- Link libraries ---
target_link_libraries(karst_4_0 PRIVATE
        VTK::CommonCore
        VTK::CommonDataModel
        VTK::IOXML
        ${Boost_LIBRARIES}
)

# --- Copy executable ---
add_custom_command(TARGET karst_4_0 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:karst_4_0> ${CMAKE_BINARY_DIR}
        COMMENT "Copying executable to build directory"
)

#cmake_minimum_required(VERSION 3.27)
#project(karst_4_0)
#
#set(CMAKE_CXX_STANDARD 23)
#
## --- Find VTK ---
#find_package(VTK REQUIRED)
#message(STATUS "VTK version: ${VTK_VERSION}")
#
## --- Include directories ---
#include_directories(
#        ${CMAKE_SOURCE_DIR}
#        src/external_algorithms
#        src/external_algorithms/triangulation
#        /opt/homebrew/opt/eigen/include/eigen3
#)
#
## --- Executable ---
#add_executable(karst_4_0 ./main.cpp)
#
## --- Link VTK with project ---
#target_link_libraries(karst_4_0 PRIVATE ${VTK_LIBRARIES})
#
## (optional) check include VTK
#target_include_directories(karst_4_0 PRIVATE ${VTK_INCLUDE_DIRS})
#message(STATUS ${VTK_DIR})
#
### Debug info
##message(STATUS "VTK Libraries: ${VTK_LIBRARIES}")
#
## --- Copy to build/ ---
#set(BUILD_DIRECTORY ./build)
#add_custom_target(copy_executable ALL
#        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:karst_4_0> ${BUILD_DIRECTORY}
#        DEPENDS karst_4_0
#        COMMENT "Copying karst_4_0 to build directory"
#)
#
#
