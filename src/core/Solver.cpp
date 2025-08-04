#include "core/Solver.hpp"
#include "data/DataLoader.hpp"
#include "exact/GurobiSolver.hpp"
#include "greedy/GreedyMDVSPSolver.hpp"
#include <chrono>
#include <iostream>

MDVSPSolver::MDVSPSolver() : problemLoaded_(false) {}

MDVSPSolver::~MDVSPSolver() = default;

bool MDVSPSolver::loadProblem(const std::string& dataPath) {
    try {
        logMessage("Loading problem from: " + dataPath);

        dataLoader_ = std::make_unique<data::DataLoader>(dataPath);

        // Load CSV data
        if (!dataLoader_->loadAll()) {
            logMessage("Failed to load CSV data", true);
            return false;
        }

        logMessage("Problem loaded successfully from CSV data");
        logMessage("  Service journeys: " + std::to_string(dataLoader_->getServiceJourneys().size()));
        logMessage("  Vehicle types: " + std::to_string(dataLoader_->getVehicleTypes().size()));
        logMessage("  Stop points: " + std::to_string(dataLoader_->getStopPoints().size()));

        problemLoaded_ = true;
        return true;
    }
    catch (const std::exception& e) {
        logMessage("Error loading problem: " + std::string(e.what()), true);
        return false;
    }
}

core::UnifiedSolution MDVSPSolver::solve(const SolverConfig& config) {
    if (!problemLoaded_) {
        logMessage("No problem loaded!", true);
        return core::UnifiedSolution();
    }

    logMessage("=== MDVSP Solver ===");
    logMessage("Method: " + config.method);
    logMessage("Time limit: " + std::to_string(config.timeLimit) + "s");
    logMessage("Problem: " + config.problemName);

    auto startTime = std::chrono::high_resolution_clock::now();

    core::UnifiedSolution solution;

    if (config.method == "greedy") {
        solution = solveGreedy(config);
    }
    else if (config.method == "exact") {
        solution = solveExact(config);
    }
    else if (config.method == "meta") {
        solution = solveMeta(config);
    }
    else {
        logMessage("Unknown method: " + config.method + ", using greedy", true);
        solution = solveGreedy(config);
    }

    auto endTime  = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    solution.solutionTimeSeconds = duration.count() / 1000.0;

    return solution;
}

core::UnifiedSolution MDVSPSolver::solve() {
    SolverConfig defaultConfig;
    defaultConfig.verbose = true;
    return solve(defaultConfig);
}

core::UnifiedSolution MDVSPSolver::solveWithWarmStart(const core::UnifiedSolution& warmStart,
                                                      const std::string&           targetMethod) {
    logMessage("=== Solving with Warm Start ===");
    logMessage("Warm start algorithm: " + warmStart.algorithmType);
    logMessage("Warm start objective: " + std::to_string(warmStart.objectiveValue));
    logMessage("Target method: " + targetMethod);

    if (targetMethod == "exact") {
        try {
            exact::GurobiSolver               gurobiSolver;
            exact::GurobiSolver::SolverConfig gurobiConfig;
            gurobiConfig.timeLimit   = 3600.0;
            gurobiConfig.verbose     = true;
            gurobiConfig.problemName = "warm_start_problem";

            // Use auto-discovery for model file
            auto solution = gurobiSolver.solveFromFileWithWarmStart("", warmStart, gurobiConfig);
            return solution;
        }
        catch (const std::exception& e) {
            logMessage("Error in Gurobi warm start: " + std::string(e.what()), true);
            return warmStart;
        }
    }

    logMessage("Warm start not supported for method: " + targetMethod);
    return warmStart;
}

core::UnifiedSolution MDVSPSolver::solveGreedy(const SolverConfig& config) {
    logMessage("Solving with greedy heuristic...");

    try {
        greedy::GreedyMDVSPSolver greedySolver(*dataLoader_);
        auto                      greedySolution = greedySolver.solve();

        core::UnifiedSolution solution;
        solution.algorithmType   = "greedy";
        solution.status          = "FEASIBLE";
        solution.objectiveValue  = greedySolution.totalObjectiveCost;
        solution.problemInstance = config.problemName;

        for (const auto& block : greedySolution.blocks) {
            core::VehicleBlock unifiedBlock;
            unifiedBlock.vehicleTypeId = block.vehicleTypeId;
            unifiedBlock.depotId       = block.depotStopId;
            unifiedBlock.journeyIds    = block.serviceJourneyIds;
            unifiedBlock.totalCost     = block.totalCost;
            unifiedBlock.totalTime     = block.totalTime;

            solution.blocks.push_back(unifiedBlock);
        }

        logMessage("Greedy solution completed successfully");
        return solution;
    }
    catch (const std::exception& e) {
        logMessage("Error in greedy solver: " + std::string(e.what()), true);
        return core::UnifiedSolution();
    }
}

core::UnifiedSolution MDVSPSolver::solveExact(const SolverConfig& config) {
    logMessage("Solving with Gurobi exact solver...");

    try {
        exact::GurobiSolver gurobiSolver;

        exact::GurobiSolver::SolverConfig gurobiConfig;
        gurobiConfig.timeLimit     = config.timeLimit;
        gurobiConfig.optimalityGap = config.optimalityGap;
        gurobiConfig.verbose       = config.verbose;
        gurobiConfig.problemName   = config.problemName;

        // Use auto-discovery for model file
        auto solution            = gurobiSolver.solveFromFile("", gurobiConfig);
        solution.problemInstance = config.problemName;

        return solution;
    }
    catch (const std::exception& e) {
        logMessage("Error in Gurobi solver: " + std::string(e.what()), true);
        return solveGreedy(config);
    }

    logMessage("Gurobi not available - using greedy fallback");
    return solveGreedy(config);
}

core::UnifiedSolution MDVSPSolver::solveMeta(const SolverConfig& config) {
    logMessage("Meta-heuristic solver not yet implemented - using greedy fallback");
    return solveGreedy(config);
}

void MDVSPSolver::logMessage(const std::string& message, bool verbose) const {
    if (!verbose) {
        std::cout << "[MDVSPSolver] " << message << std::endl;
    }
}
