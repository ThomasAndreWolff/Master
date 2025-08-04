#include "exact/Config.hpp"
#include "exact/GurobiSolver.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>

// Simple INI parser for configuration
std::unordered_map<std::string, std::string> parseConfigFile(const std::string& filename) {
    std::unordered_map<std::string, std::string> config;
    std::ifstream                                file(filename);

    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << filename << std::endl;
        return config;
    }

    std::string line;
    std::string currentSection;

    while (std::getline(file, line)) {
        // Remove whitespace
        line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#')
            continue;

        // Section header
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }

        // Key-value pair
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key   = std::regex_replace(line.substr(0, pos), std::regex("\\s+$"), "");
            std::string value = std::regex_replace(line.substr(pos + 1), std::regex("^\\s+"), "");

            // Store with section prefix
            if (!currentSection.empty()) {
                key = currentSection + "." + key;
            }
            config[key] = value;
        }
    }

    return config;
}

int main(int argc, char* argv[]) {
    std::cout << "=== Exact MDVSP Solver (Gurobi) ===" << std::endl;
    std::cout << "High-precision solver for the Multi-Depot Vehicle Scheduling Problem" << std::endl;
    std::cout << std::endl;

    // Load configuration file
    std::string configFile = "/workspace/config/mdvsp_config.ini";
    if (argc > 1) {
        configFile = argv[1];
    }

    std::cout << "Loading configuration from: " << configFile << std::endl;
    auto config = parseConfigFile(configFile);
    std::cout << std::endl;

    // Extract configuration values with defaults
    std::string modelFile = config.count("problem.input_file")
                                ? config["problem.input_file"]
                                : "/workspace/models/ffdhhavhh_aggltp_1-2max.lp";
    double timeLimit =
        config.count("solver.time_limit") ? std::stod(config["solver.time_limit"]) : 3600.0;
    double optimalityGap =
        config.count("solver.mip_gap") ? std::stod(config["solver.mip_gap"]) : 0.01;
    int  threads  = config.count("solver.threads") ? std::stoi(config["solver.threads"]) : 0;
    int  presolve = config.count("solver.presolve") ? std::stoi(config["solver.presolve"]) : 2;
    bool verbose =
        config.count("solver.log_to_console") ? (config["solver.log_to_console"] == "true") : true;
    std::string logFile = config.count("output.log_file") ? config["output.log_file"]
                                                          : "/workspace/results/gurobi.log";
    std::string solutionFile = config.count("output.solution_file")
                                   ? config["output.solution_file"]
                                   : "/workspace/results/mdvsp_solution.txt";

    // Print configuration
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Model file: " << modelFile << std::endl;
    std::cout << "  Time limit: " << timeLimit << "s" << std::endl;
    std::cout << "  Optimality gap: " << (optimalityGap * 100) << "%" << std::endl;
    std::cout << "  Threads: " << (threads == 0 ? "auto" : std::to_string(threads)) << std::endl;
    std::cout << "  Presolve: " << presolve << std::endl;
    std::cout << "  Log file: " << logFile << std::endl;
    std::cout << "  Solution file: " << solutionFile << std::endl;
    std::cout << std::endl;

    try {
        // Initialize Gurobi solver
        exact::GurobiSolver solver;

        // Configure solver
        exact::GurobiSolver::SolverConfig solverConfig;
        solverConfig.problemName   = "mdvsp";
        solverConfig.modelFile     = modelFile;
        solverConfig.timeLimit     = timeLimit;
        solverConfig.optimalityGap = optimalityGap;
        solverConfig.verbose       = verbose;
        solverConfig.logFile       = logFile;

        // Solve the problem
        std::cout << "Starting exact optimization..." << std::endl;
        auto solution = solver.solveFromFile(modelFile, solverConfig);

        // Print results
        std::cout << "\n=== Exact Solution Results ===" << std::endl;
        std::cout << "Algorithm: " << solution.algorithmType << std::endl;
        std::cout << "Status: " << solution.status << std::endl;

        if (solution.status == "OPTIMAL" || solution.status == "TIME_LIMIT") {
            std::cout << "Objective: " << std::fixed << std::setprecision(2)
                      << solution.objectiveValue << std::endl;
        }

        std::cout << "Solution time: " << std::fixed << std::setprecision(3)
                  << solution.solutionTimeSeconds << "s" << std::endl;
        std::cout << "Problem instance: " << solution.problemInstance << std::endl;

        // Success codes
        if (solution.status == "OPTIMAL") {
            std::cout << "\n✓ Optimal solution found!" << std::endl;
            return 0;
        }
        else if (solution.status == "TIME_LIMIT") {
            std::cout << "\n⚠ Time limit reached with best solution found" << std::endl;
            return 0;
        }
        else if (solution.status == "INFEASIBLE") {
            std::cout << "\n✗ Problem is infeasible" << std::endl;
            return 1;
        }
        else {
            std::cout << "\n? Unknown status: " << solution.status << std::endl;
            return 2;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "\nERROR: " << e.what() << std::endl;
        std::cerr << "Make sure Gurobi is properly installed and model files exist." << std::endl;
        return 3;
    }
}
