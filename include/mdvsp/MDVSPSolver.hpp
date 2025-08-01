#pragma once

#include "MDVSPAnalyzer.hpp"
#include "MDVSPConfigurator.hpp"
#include "MDVSPFileHandler.hpp"
#include <memory>
#include <string>
#include <vector>
#include "gurobi_c++.h"

/**
 * Multi-Depot Vehicle Scheduling Problem (MDVSP) Solver using Gurobi
 *
 * This solver handles:
 * - Depot assignment (return to start depot)
 * - Depot capacities (limited parking spaces)
 * - Vehicle type compatibility (which bus can handle which trip)
 * - Heterogeneous cost structures (different vehicle costs)
 * - Routing rules (direct vs. depot deadhead trips)
 */
class MDVSPSolver {
  public:
    struct SolutionInfo {
        double objectiveValue;
        int    status;
        double solutionTime;
        int    numVariables;
        int    numConstraints;
        bool   isFeasible;
        bool   isOptimal;

        // Solution details
        std::vector<std::vector<int>> vehicleRoutes;    // Routes for each vehicle
        std::vector<int>              depotAssignments; // Which depot each vehicle belongs to
        std::vector<double>           totalCosts;       // Cost per vehicle
    };

    MDVSPSolver();
    ~MDVSPSolver();

    /**
     * Load MDVSP problem from MPS file
     * @param filename Path to the MPS file
     * @return true if successful, false otherwise
     */
    bool loadFromMPS(const std::string& filename);

    /**
     * Load MDVSP problem from LP file
     * @param filename Path to the LP file
     * @return true if successful, false otherwise
     */
    bool loadFromLP(const std::string& filename);

    /**
     * Load MDVSP problem from file (auto-detects format)
     * Supports both MPS and LP formats
     * @param filename Path to the problem file
     * @return true if successful, false otherwise
     */
    bool loadFromFile(const std::string& filename);

    /**
     * Solve the loaded MDVSP problem
     * @param timeLimit Time limit in seconds (0 = no limit)
     * @return Solution information
     */
    SolutionInfo solve(double timeLimit = 0.0);

    /**
     * Print solution summary
     */
    void printSolution(const SolutionInfo& solution);

    /**
     * Export solution to file
     */
    bool exportSolution(const SolutionInfo& solution, const std::string& filename);

  private:
    std::unique_ptr<GRBEnv>   env_;
    std::unique_ptr<GRBModel> model_;

    // Problem dimensions
    int numVehicles_;
    int numTrips_;
    int numDepots_;

    // Variables and constraints tracking
    std::vector<GRBVar> vehicleVars_;
    std::vector<GRBVar> routingVars_;
    std::vector<GRBVar> depotVars_;

    // Specialized components
    std::unique_ptr<MDVSPFileHandler>  fileHandler_;
    std::unique_ptr<MDVSPAnalyzer>     analyzer_;
    std::unique_ptr<MDVSPConfigurator> configurator_;
};
