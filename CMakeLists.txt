cmake_minimum_required(VERSION 3.10)
project(mdvsp_solver)

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

# Include directories
include_directories(include)

# Executable
add_executable(mdvsp_solver
    src/main.cpp
    src/mdvsp/solver/MDVSPSolver.cpp
    src/mdvsp/utils/MDVSPUtils.cpp
    src/mdvsp/file/MDVSPFileHandler.cpp
    src/mdvsp/analysis/MDVSPAnalyzer.cpp
    src/mdvsp/config/MDVSPConfigurator.cpp
)

target_include_directories(mdvsp_solver PRIVATE include)

# BESTE LÖSUNG: Verwende die vorgefertigten Conan Config-Files
# Diese Methode funktioniert für ALLE Conan-Pakete automatisch!

# 1. CMAKE_PREFIX_PATH für Conan-Pakete setzen
list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/conan_build")

# 2. find_package für alle benötigten Pakete
find_package(SQLiteCpp REQUIRED)

# 3. Verwende die automatisch erstellten Targets
target_link_libraries(mdvsp_solver PRIVATE SQLiteCpp)

# Link gegen Gurobi
target_link_libraries(mdvsp_solver PRIVATE gurobi_c++ gurobi120 pthread m)
