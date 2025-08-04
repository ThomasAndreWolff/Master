#pragma once

#include "core/UnifiedSolution.hpp"
#include "exact/Callback.hpp"
#include "exact/Config.hpp"
#include <memory>
#include <string>
#include "gurobi_c++.h"

namespace exact {

/**
 * Gurobi-based exact solver for MDVSP
 *
 * This solver handles:
 * - Multi-depot vehicle scheduling
 * - Vehicle type compatibility
 * - Depot capacities
 * - Exact optimization using Mixed Integer Programming
 */
class GurobiSolver {
  public:
    /**
     * Configuration for exact solver
     */
    struct SolverConfig {
        double      timeLimit     = 3600.0;       // Time limit in seconds
        double      optimalityGap = 0.01;         // MIP gap (1%)
        int         threads       = 0;            // Number of threads (0 = auto)
        bool        verbose       = false;        // Enable Gurobi output
        bool        useCallback   = true;         // Use progress callback
        std::string logFile       = "gurobi.log"; // Log file path
        std::string problemName   = "mdvsp";      // Problem instance name
        std::string modelFile     = "";           // Path to model file (LP/MPS)

        // Model file discovery
        std::string modelDirectory  = "models"; // Directory to search for models
        std::string preferredFormat = ".lp";    // Preferred file format (.lp or .mps)
    };

    /**
     * Constructor
     */
    GurobiSolver();

    /**
     * Destructor
     */
    ~GurobiSolver();

    /**
     * Load and solve a model from file (LP/MPS format)
     * @param filename Path to model file
     * @param config Solver configuration
     * @return Unified solution
     */
    core::UnifiedSolution solveFromFile(const std::string& filename, const SolverConfig& config);

    /**
     * Solve with warm start from another solution
     * @param filename Path to model file
     * @param warmStart Initial solution to improve
     * @param config Solver configuration
     * @return Improved solution
     */
    core::UnifiedSolution solveFromFileWithWarmStart(const std::string&           filename,
                                                     const core::UnifiedSolution& warmStart,
                                                     const SolverConfig&          config);

    /**
     * Load problem from MPS/LP file (alternative to DataLoader)
     * @param filename Path to optimization model file
     * @return true if successful
     */
    bool loadFromFile(const std::string& filename);

  private:
    std::unique_ptr<GRBEnv>             env_;
    std::unique_ptr<GRBModel>           model_;
    std::unique_ptr<SolverCallback>     callback_;
    std::unique_ptr<SolverConfigurator> configurator_;

    // Problem state
    bool problemBuilt_;

    core::UnifiedSolution extractSolution();
    void                  setWarmStart(const core::UnifiedSolution& warmStart);

    void logMessage(const std::string& message, bool verbose = false) const;
};

} // namespace exact
