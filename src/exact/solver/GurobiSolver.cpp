#include "exact/GurobiSolver.hpp"
#include "core/UnifiedSolution.hpp"
#include "exact/Config.hpp"
#include <chrono>
#include <iostream>

namespace exact {

using core::UnifiedSolution;

GurobiSolver::GurobiSolver() : problemBuilt_(false) {
    try {
        // Initialize Gurobi environment
        env_ = std::make_unique<GRBEnv>();
        env_->set(GRB_IntParam_LogToConsole, 0); // Disable console output by default
        env_->start();

        // Initialize configurator
        configurator_ = std::make_unique<SolverConfigurator>();

        logMessage("Gurobi Solver initialized successfully");
    }
    catch (GRBException& e) {
        logMessage("Gurobi exception during initialization: " + std::string(e.getMessage()), true);
        throw std::runtime_error("Failed to initialize Gurobi environment");
    }
    catch (const std::exception& e) {
        logMessage("Error initializing Gurobi solver: " + std::string(e.what()), true);
        throw;
    }
}

GurobiSolver::~GurobiSolver() {
    try {
        model_.reset();
        env_.reset();
    }
    catch (...) {
        // Ignore exceptions in destructor
    }
}

bool GurobiSolver::loadFromFile(const std::string& filename) {
    try {
        logMessage("Loading optimization model from: " + filename);

        model_        = std::make_unique<GRBModel>(*env_, filename);
        problemBuilt_ = true;

        logMessage("Model loaded successfully from file");
        logMessage("  Variables: " + std::to_string(model_->get(GRB_IntAttr_NumVars)));
        logMessage("  Constraints: " + std::to_string(model_->get(GRB_IntAttr_NumConstrs)));

        return true;
    }
    catch (GRBException& e) {
        logMessage("Gurobi error loading file: " + std::string(e.getMessage()), true);
        return false;
    }
    catch (const std::exception& e) {
        logMessage("Error loading file: " + std::string(e.what()), true);
        return false;
    }
}

UnifiedSolution GurobiSolver::solveFromFile(const std::string& filename, const SolverConfig& config) {
    auto startTime = std::chrono::high_resolution_clock::now();

    try {
        logMessage("=== Gurobi Exact Solver ===");

        // Determine model file to use
        std::string modelFile = filename;
        if (modelFile.empty() && !config.modelFile.empty()) {
            modelFile = config.modelFile;
        }
        if (modelFile.empty()) {
            // Auto-discover model file
            modelFile = SolverConfigurator::findModelFile(
                config.problemName, config.modelDirectory, config.preferredFormat);
        }

        if (modelFile.empty()) {
            logMessage("No model file specified or found", true);
            return UnifiedSolution();
        }

        logMessage("Model file: " + modelFile);
        logMessage("Time limit: " + std::to_string(config.timeLimit) + "s");
        logMessage("Optimality gap: " + std::to_string(config.optimalityGap * 100) + "%");

        // Load model from file
        if (!loadFromFile(modelFile)) {
            logMessage("Failed to load model from file", true);
            return UnifiedSolution();
        }

        // Configure solver
        configurator_->configureSolver(model_.get());
        configurator_->setTimeLimit(model_.get(), config.timeLimit);

        // Apply additional config settings
        model_->set(GRB_DoubleParam_MIPGap, config.optimalityGap);
        model_->set(GRB_IntParam_LogToConsole, config.verbose ? 1 : 0);
        model_->set(GRB_StringParam_LogFile, config.logFile);

        if (config.threads > 0) {
            model_->set(GRB_IntParam_Threads, config.threads);
        }

        // Solve the model
        logMessage("Starting optimization...");
        model_->optimize();

        // Extract and return solution
        auto solution            = extractSolution();
        solution.algorithmType   = "exact_gurobi";
        solution.problemInstance = config.problemName;

        auto endTime  = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        solution.solutionTimeSeconds = duration.count() / 1000.0;

        return solution;
    }
    catch (GRBException& e) {
        logMessage("Gurobi optimization error: " + std::string(e.getMessage()), true);
        return UnifiedSolution();
    }
    catch (const std::exception& e) {
        logMessage("Error during optimization: " + std::string(e.what()), true);
        return UnifiedSolution();
    }
}

UnifiedSolution GurobiSolver::solveFromFileWithWarmStart(const std::string&     filename,
                                                         const UnifiedSolution& warmStart,
                                                         const SolverConfig&    config) {
    logMessage("=== Solving with Warm Start ===");
    logMessage("Warm start objective: " + std::to_string(warmStart.objectiveValue));

    try {
        // Load model
        if (!loadFromFile(filename)) {
            logMessage("Failed to load model for warm start", true);
            return warmStart;
        }

        // Set warm start values (simplified - would need actual variable mapping)
        logMessage("Setting warm start values...");
        // TODO: Implement warm start value setting based on variable names
        logMessage("Warm start values set (placeholder implementation)");

        // Solve with warm start
        return solveFromFile(filename, config);
    }
    catch (const std::exception& e) {
        logMessage("Error in warm start solving: " + std::string(e.what()), true);
        return warmStart;
    }
}

core::UnifiedSolution GurobiSolver::extractSolution() {
    UnifiedSolution solution;
    solution.algorithmType = "exact_gurobi";

    try {
        int status = model_->get(GRB_IntAttr_Status);

        if (status == GRB_OPTIMAL) {
            solution.status         = "OPTIMAL";
            solution.objectiveValue = model_->get(GRB_DoubleAttr_ObjVal);

            logMessage("Optimal solution found!");
            logMessage("  Objective value: " + std::to_string(solution.objectiveValue));
        }
        else if (status == GRB_TIME_LIMIT) {
            solution.status = "TIME_LIMIT";
            if (model_->get(GRB_IntAttr_SolCount) > 0) {
                solution.objectiveValue = model_->get(GRB_DoubleAttr_ObjVal);
                logMessage("Time limit reached, best solution found: " +
                           std::to_string(solution.objectiveValue));
            }
        }
        else if (status == GRB_INFEASIBLE) {
            solution.status = "INFEASIBLE";
            logMessage("Problem is infeasible", true);
        }
        else {
            solution.status = "UNKNOWN";
            logMessage("Solver status: " + std::to_string(status), true);
        }

        // Extract variable values (simplified - would need to match variable names to solution
        // structure)
        if (solution.status == "OPTIMAL" || solution.status == "TIME_LIMIT") {
            logMessage("Solution extraction from Gurobi variables - placeholder implementation");
            // TODO: Map Gurobi variables back to UnifiedSolution blocks
        }
    }
    catch (GRBException& e) {
        logMessage("Error extracting solution: " + std::string(e.getMessage()), true);
        solution.status = "ERROR";
    }

    return solution;
}

void GurobiSolver::logMessage(const std::string& message, bool verbose) const {
    if (!verbose) {
        std::cout << "[GurobiSolver] " << message << std::endl;
    }
}

} // namespace exact
