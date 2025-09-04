# --- Basic Project Setup ---
cmake_minimum_required(VERSION 3.27)
project(karst_4_0 LANGUAGES CXX)

# Acknowledge that the old FindBoost module is deprecated and we are using the modern config-based approach.
cmake_policy(SET CMP0167 NEW)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the output directory for runtime artifacts (e.g., executables)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# --- Dependency Discovery ---
find_package(VTK 9.5 REQUIRED COMPONENTS
        CommonCore
        CommonDataModel
        IOXML
)
message(STATUS "Found VTK version: ${VTK_VERSION}")

find_package(Eigen3 REQUIRED)
message(STATUS "Found Eigen3 version: ${EIGEN3_VERSION_STRING}")

find_package(Boost 1.81 REQUIRED COMPONENTS serialization) # Adjust version if needed
message(STATUS "Found Boost version: ${Boost_VERSION_STRING}")

find_package(cxxopts REQUIRED)
message(STATUS "Found cxxopts version: ${cxxopts_VERSION}")
# --- Target Definition (Executable) ---
add_executable(karst_4_0 main.cpp)

# --- Configure Include Directories for our Target ---
target_include_directories(karst_4_0 PRIVATE
        ${CMAKE_SOURCE_DIR}
        src
        src/external_algorithms
        src/external_algorithms/triangulation
        # Note: Eigen3 and Boost targets will add their include paths automatically during linking
)

# --- Configure Library Linking for our Target ---
target_link_libraries(karst_4_0 PRIVATE
        VTK::CommonCore
        VTK::CommonDataModel
        VTK::IOXML
        Eigen3::Eigen
        Boost::serialization
        cxxopts::cxxopts
)

# --- Configure Compile Options for our Target ---
# Generic, configuration-independent options can be placed here
# target_compile_options(...)

# Options specific to the Debug configuration
target_compile_options(karst_4_0 PRIVATE
        $<$<CONFIG:Debug>:-g -O0 -ggdb>
)

# Options specific to Release (and other release-like) configurations
target_compile_options(karst_4_0 PRIVATE
        $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:-O3 -DNDEBUG>
)

# Enable AddressSanitizer (ASan) in Debug mode if the compiler is Clang
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(karst_4_0 PRIVATE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer>)
    target_link_options(karst_4_0 PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
endif()

message(STATUS "Configuration for target 'karst_4_0' is complete.")