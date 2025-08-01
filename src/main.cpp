#include "mdvsp/MDVSPSolver.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main() {
    try {
        std::cout << "==================================================" << std::endl;
        std::cout << "Multi-Depot Vehicle Scheduling Problem (MDVSP) Solver" << std::endl;
        std::cout << "Using Gurobi Optimization" << std::endl;
        std::cout << "==================================================" << std::endl;

        // Path to the problem file (supports both MPS and LP formats)
        std::string problemFile = "/workspace/models/ffdhhavhh_aggltp_1-2max.lp";

        // Check if file exists
        std::ifstream file(problemFile);
        if (!file.good()) {
            std::cerr << "Error: Problem file not found: " << problemFile << std::endl;
            return 1;
        }
        file.close();

        std::cout << "\nLoading large optimization problem..." << std::endl;

        // Create solver instance
        std::cout << "\nInitializing MDVSP Solver..." << std::endl;
        MDVSPSolver solver;

        // Load the problem (auto-detects format)
        std::cout << "\nLoading MDVSP problem..." << std::endl;
        if (!solver.loadFromFile(problemFile)) {
            std::cerr << "Failed to load problem file: " << problemFile << std::endl;
            return 1;
        }

        // Set time limit for large problems
        double timeLimit = 3600.0; // 1 hour
        std::cout << "\nSetting time limit to " << timeLimit << " seconds" << std::endl;

        // Solve the problem
        std::cout << "\nStarting optimization..." << std::endl;
        auto solution = solver.solve(timeLimit);

        // Export solution if feasible
        if (solution.isFeasible) {
            std::string solutionFile = "/workspace/results/mdvsp_solution.txt";
            std::cout << "\nExporting solution..." << std::endl;
            solver.exportSolution(solution, solutionFile);
        }

        // Print final results
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "MDVSP OPTIMIZATION COMPLETE" << std::endl;
        std::cout << std::string(50, '=') << std::endl;

        if (solution.isOptimal) {
            std::cout << "✓ Optimal solution found!" << std::endl;
            std::cout << "Objective value: " << solution.objectiveValue << std::endl;
        }
        else if (solution.isFeasible) {
            std::cout << "✓ Feasible solution found (may not be optimal)" << std::endl;
            std::cout << "Objective value: " << solution.objectiveValue << std::endl;
        }
        else {
            std::cout << "✗ No feasible solution found" << std::endl;

            // Suggest troubleshooting steps
            std::cout << "\nTroubleshooting suggestions:" << std::endl;
            std::cout << "1. Check problem feasibility constraints" << std::endl;
            std::cout << "2. Increase time limit for large problems" << std::endl;
            std::cout << "3. Review depot capacities and vehicle compatibility" << std::endl;
            std::cout << "4. Check if all trips can be covered by available vehicles" << std::endl;
        }

        std::cout << "Total solving time: " << solution.solutionTime << " seconds" << std::endl;

        return solution.isFeasible ? 0 : 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown exception occurred" << std::endl;
        return 1;
    }
}
