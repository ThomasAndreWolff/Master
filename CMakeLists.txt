cmake_minimum_required(VERSION 3.10)
project(gurobi_app)

# Conan Toolchain einbinden (beim CMake-Aufruf mit -DCMAKE_TOOLCHAIN_FILE=conan_build/conan_toolchain.cmake)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Find Gurobi
set(GUROBI_HOME "/opt/gurobi1201/linux64")
include_directories(${GUROBI_HOME}/include)
link_directories(${GUROBI_HOME}/lib)

# ParadisEO
set(PARADISEO_DIR /opt/paradiseo)
set(PARADISEO_LIB_DIR ${PARADISEO_DIR}/build/lib)
include_directories(
  /opt/paradiseo/eo/src
  /opt/paradiseo/mo/src
  /opt/paradiseo/moeo/src
)
link_directories(${PARADISEO_LIB_DIR})

# Executable
add_executable(gurobi_app
    src/main.cpp
    src/reader/TSPLibReader.cpp
    src/solver/exact/TSPSolver.cpp
    src/solver/heuristic/SimulatedAnnealingSolver.cpp
    src/solver/heuristic/SimulatedAnnealingParadiseo.cpp 
    src/solver/genetic/GeneticAlgorithmParadiseo.cpp 
    src/logger/TSPResultLogger.cpp
    src/mutation/SwapMutation.cpp
    src/mutation/ReverseMutation.cpp
)

target_include_directories(gurobi_app PRIVATE src)

# Add Conan build dir to CMAKE_MODULE_PATH for find_package(SQLiteCpp)
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/../conan_build")

# Conan 2.x: Finde und linke SQLiteCpp
find_package(SQLiteCpp REQUIRED)

target_link_libraries(gurobi_app PRIVATE SQLiteCpp)

# Link gegen Gurobi
# (optional, falls nicht schon über Conan abgedeckt)
target_link_libraries(gurobi_app PRIVATE gurobi_c++ gurobi120 pthread m)

# Link ParadisEO
set(PARADISEO_LIBS eo moeo eoutils)
target_link_libraries(gurobi_app PRIVATE ${PARADISEO_LIBS})
