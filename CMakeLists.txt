cmake_minimum_required(VERSION 3.15)
project(mdvsp_solver)

# Projektweite Einstellungen
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build-Typ (Debug/Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Gurobi: Pfad manuell setzen
set(GUROBI_HOME "/opt/gurobi1201/linux64")
include_directories(${GUROBI_HOME}/include)
link_directories(${GUROBI_HOME}/lib)

# Include directories
include_directories(include)

# Find packages
find_package(SQLiteCpp REQUIRED)

# === Three Main Executables ===

# 1. Greedy Solver - Fast heuristic algorithm
add_executable(greedy_mdvsp
    src/main_greedy.cpp
    src/data/DataLoader.cpp
    src/greedy/solver/GreedySolver.cpp
    src/greedy/analysis/GreedySolutionPrinter.cpp
    src/core/UnifiedSolution.cpp
    src/core/SolutionTransformer.cpp
    src/core/database/UnifiedSolutionDatabase.cpp
)
target_link_libraries(greedy_mdvsp PRIVATE SQLiteCpp pthread)

# 2. Exact Solver - Gurobi-based optimization (requires model files)
add_executable(exact_mdvsp
    src/main_exact.cpp
    src/exact/solver/GurobiSolver.cpp
    src/exact/callback/Callback.cpp
    src/exact/config/Config.cpp
    src/core/UnifiedSolution.cpp
)
target_link_libraries(exact_mdvsp PRIVATE gurobi_c++ gurobi120 pthread)

# 3. Unified Solver - Combined greedy + exact with warm start
add_executable(mdvsp_unified
    src/main_unified.cpp
    src/core/Solver.cpp
    src/core/UnifiedSolution.cpp
    src/core/SolutionConverter.cpp
    src/data/DataLoader.cpp
    src/greedy/solver/GreedySolver.cpp
    src/exact/solver/GurobiSolver.cpp
    src/exact/callback/Callback.cpp
    src/exact/config/Config.cpp
)
target_link_libraries(mdvsp_unified PRIVATE SQLiteCpp gurobi_c++ gurobi120 pthread)

# === Configuration Summary ===
message(STATUS "")
message(STATUS "MDVSP Solver Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Gurobi path: ${GUROBI_HOME}")
message(STATUS "")
message(STATUS "Three Main Executables:")
message(STATUS "  1. greedy_mdvsp   - Fast greedy heuristic")
message(STATUS "  2. exact_mdvsp    - Exact Gurobi solver (requires .lp/.mps files)")
message(STATUS "  3. mdvsp_unified  - Combined greedy + exact with warm start")
message(STATUS "")
