#include "mdvsp/MDVSPSolver.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>

MDVSPSolver::MDVSPSolver() {
    try {
        // Create default environment without custom parameters first
        env_ = std::make_unique<GRBEnv>();
        
        // Only set essential logging parameters
        env_->set(GRB_IntParam_LogToConsole, 1);
        env_->set(GRB_IntParam_OutputFlag, 1);
        env_->set(GRB_StringParam_LogFile, "gurobi.log");

        model_ = std::make_unique<GRBModel>(*env_);

        // Initialize problem dimensions
        numVehicles_ = 0;
        numTrips_    = 0;
        numDepots_   = 0;

        // Initialize specialized components
        fileHandler_  = std::make_unique<MDVSPFileHandler>();
        analyzer_     = std::make_unique<MDVSPAnalyzer>();
        configurator_ = std::make_unique<MDVSPConfigurator>();

        std::cout << "MDVSP Solver initialized with Gurobi successfully" << std::endl;
    }
    catch (GRBException& e) {
        std::cerr << "Gurobi exception during initialization:" << std::endl;
        std::cerr << "  Message: " << e.getMessage() << std::endl;
        std::cerr << "  Error code: " << e.getErrorCode() << std::endl;
        throw;
    }
}

MDVSPSolver::~MDVSPSolver() {
    // Unique pointers will handle cleanup automatically
}

bool MDVSPSolver::loadFromMPS(const std::string& filename) {
    try {
        std::cout << "Loading MPS file..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        model_ = std::make_unique<GRBModel>(*env_, filename);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        std::cout << "File loaded in " << duration.count() << " seconds" << std::endl;

        // Analyze and configure
        analyzer_->analyzeProblemStructure(model_.get());
        analyzer_->printProblemStatistics(model_.get());
        configurator_->configureSolver(model_.get());

        return true;
    }
    catch (GRBException& e) {
        std::cerr << "Error loading MPS file: " << e.getMessage() << std::endl;
        return false;
    }
}

bool MDVSPSolver::loadFromLP(const std::string& filename) {
    try {
        std::cout << "Loading LP file..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        model_ = std::make_unique<GRBModel>(*env_, filename);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        std::cout << "File loaded in " << duration.count() << " seconds" << std::endl;

        // Analyze and configure
        analyzer_->analyzeProblemStructure(model_.get());
        analyzer_->printProblemStatistics(model_.get());
        configurator_->configureSolver(model_.get());

        return true;
    }
    catch (GRBException& e) {
        std::cerr << "Error loading LP file: " << e.getMessage() << std::endl;
        return false;
    }
}

bool MDVSPSolver::loadFromFile(const std::string& filename) {
    try {
        std::cout << "Loading problem file..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        model_ = std::make_unique<GRBModel>(*env_, filename);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        std::cout << "File loaded in " << duration.count() << " seconds" << std::endl;

        // Analyze and configure
        analyzer_->analyzeProblemStructure(model_.get());
        analyzer_->printProblemStatistics(model_.get());
        configurator_->configureSolver(model_.get());

        return true;
    }
    catch (GRBException& e) {
        std::cerr << "Error loading problem file: " << e.getMessage() << std::endl;
        return false;
    }
}

MDVSPSolver::SolutionInfo MDVSPSolver::solve(double timeLimit) {
    SolutionInfo solution;

    try {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "Starting MDVSP optimization..." << std::endl;
        std::cout << std::string(50, '=') << std::endl;

        // Set time limit if specified using configurator
        if (timeLimit > 0.0) {
            configurator_->setTimeLimit(model_.get(), timeLimit);
            std::cout << "Time limit set to " << timeLimit << " seconds" << std::endl;
        }

        auto start = std::chrono::high_resolution_clock::now();

        // Optimize the model
        model_->optimize();

        auto end              = std::chrono::high_resolution_clock::now();
        auto duration         = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        solution.solutionTime = duration.count() / 1000.0;

        // Get solution status
        solution.status         = model_->get(GRB_IntAttr_Status);
        solution.numVariables   = model_->get(GRB_IntAttr_NumVars);
        solution.numConstraints = model_->get(GRB_IntAttr_NumConstrs);

        // Check if we have a solution
        if (solution.status == GRB_OPTIMAL) {
            solution.isOptimal      = true;
            solution.isFeasible     = true;
            solution.objectiveValue = model_->get(GRB_DoubleAttr_ObjVal);
            std::cout << "\nOptimal solution found!" << std::endl;
        }
        else if (solution.status == GRB_SUBOPTIMAL) {
            solution.isOptimal      = false;
            solution.isFeasible     = true;
            solution.objectiveValue = model_->get(GRB_DoubleAttr_ObjVal);
            std::cout << "\nFeasible solution found (not proven optimal)." << std::endl;
        }
        else {
            solution.isOptimal      = false;
            solution.isFeasible     = false;
            solution.objectiveValue = GRB_INFINITY;

            std::cout << "\nNo feasible solution found. Status: ";
            switch (solution.status) {
                case GRB_INFEASIBLE:
                    std::cout << "INFEASIBLE" << std::endl;
                    break;
                case GRB_INF_OR_UNBD:
                    std::cout << "INFEASIBLE OR UNBOUNDED" << std::endl;
                    break;
                case GRB_UNBOUNDED:
                    std::cout << "UNBOUNDED" << std::endl;
                    break;
                case GRB_TIME_LIMIT:
                    std::cout << "TIME LIMIT REACHED" << std::endl;
                    break;
                default:
                    std::cout << "OTHER (" << solution.status << ")" << std::endl;
            }
        }

        // Extract solution details if we have a feasible solution using analyzer
        if (solution.isFeasible) {
            int activeVars = analyzer_->extractSolutionDetails(model_.get());
            std::cout << "Active variables in solution: " << activeVars << std::endl;
        }

        // Print solution summary
        printSolution(solution);
    }
    catch (GRBException& e) {
        std::cerr << "Gurobi exception during optimization: " << e.getMessage() << std::endl;
        solution.status     = -1;
        solution.isFeasible = false;
        solution.isOptimal  = false;
    }

    return solution;
}

void MDVSPSolver::printSolution(const SolutionInfo& solution) {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "SOLUTION SUMMARY" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Status: ";
    if (solution.isOptimal) {
        std::cout << "OPTIMAL" << std::endl;
    }
    else if (solution.isFeasible) {
        std::cout << "FEASIBLE" << std::endl;
    }
    else {
        std::cout << "NO SOLUTION" << std::endl;
    }

    if (solution.isFeasible) {
        std::cout << "Objective Value: " << solution.objectiveValue << std::endl;
    }

    std::cout << "Solution Time: " << solution.solutionTime << " seconds" << std::endl;
    std::cout << "Variables: " << solution.numVariables << std::endl;
    std::cout << "Constraints: " << solution.numConstraints << std::endl;

    std::cout << std::string(50, '=') << std::endl;
}

bool MDVSPSolver::exportSolution(const SolutionInfo& solution, const std::string& filename) {
    try {
        if (!solution.isFeasible) {
            std::cerr << "Cannot export infeasible solution." << std::endl;
            return false;
        }

        // Use file handler to export the solution
        return fileHandler_->exportSolution(
            model_.get(), filename, solution.objectiveValue, solution.solutionTime, solution.isOptimal);
    }
    catch (std::exception& e) {
        std::cerr << "Error exporting solution: " << e.what() << std::endl;
        return false;
    }
}
