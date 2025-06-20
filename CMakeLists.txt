cmake_minimum_required(VERSION 3.10)
project(gurobi_app)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Find Gurobi
set(GUROBI_HOME "/opt/gurobi1201/linux64")
include_directories(${GUROBI_HOME}/include)
link_directories(${GUROBI_HOME}/lib)

# Include directories
include_directories(${GUROBI_INCLUDE_DIRS} src)

# Add ParadisEO include and library paths
set(PARADISEO_DIR /opt/paradiseo)
set(PARADISEO_LIB_DIR ${PARADISEO_DIR}/build/lib)

include_directories(
  /opt/paradiseo/eo/src
  /opt/paradiseo/mo/src
  /opt/paradiseo/moeo/src
)
link_directories(${PARADISEO_LIB_DIR})

# Add the executable
add_executable(gurobi_app
    src/main.cpp
    src/reader/TSPLibReader.cpp
    src/solver/exact/TSPSolver.cpp
    src/solver/heuristic/SimulatedAnnealingSolver.cpp
    src/solver/heuristic/SimulatedAnnealingParadiseo.cpp 
    src/solver/genetic/GeneticAlgorithmParadiseo.cpp 
    src/mutation/SwapMutation.cpp
    src/mutation/ReverseMutation.cpp
)

# Link against Gurobi libraries
target_link_libraries(gurobi_app gurobi_c++ gurobi120 pthread m)

# Link ParadisEO libraries
set(PARADISEO_LIBS eo moeo eoutils)
target_link_libraries(gurobi_app ${PARADISEO_LIBS})

# Include Conan
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_BINARY_DIR}/conan_toolchain.cmake" CACHE FILEPATH "Conan toolchain file")

