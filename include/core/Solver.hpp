#pragma once

#include "core/UnifiedSolution.hpp"
#include "data/DataLoader.hpp"
#include <memory>
#include <string>

// Forward declarations
namespace core {
class UnifiedSolution;
class VehicleBlock;
} // namespace core

namespace data {
class DataLoader;
}

/**
 * MDVSP Solver with algorithm interoperability
 *
 * Supports multiple solving methods:
 * - Greedy heuristic (fast, good quality)
 * - Exact optimization (optimal, slower)
 * - Meta-heuristics (balanced)
 *
 * Uses UnifiedSolution format for seamless algorithm integration
 * and warm start capabilities between different solving methods.
 */
class MDVSPSolver {
  public:
    /**
     * Configuration for solver behavior
     */
    struct SolverConfig {
        std::string method      = "greedy"; // "greedy", "exact", "meta"
        double      timeLimit   = 60.0;     // Time limit in seconds
        bool        verbose     = false;    // Enable detailed logging
        std::string problemName = "mdvsp";  // Problem instance name

        // Algorithm-specific parameters
        int    maxIterations = 1000; // For meta-heuristics
        double optimalityGap = 0.01; // For exact solver (1% gap)
    };

    MDVSPSolver();
    ~MDVSPSolver();

    /**
     * Load MDVSP problem from CSV data directory
     * @param dataPath Path to directory containing CSV files
     * @return true if successful, false otherwise
     */
    bool loadProblem(const std::string& dataPath);

    /**
     * Solve the loaded problem with configuration
     * @param config Solver configuration
     * @return Solution in unified format
     */
    core::UnifiedSolution solve(const SolverConfig& config);

    /**
     * Solve with default greedy method
     * @return Solution in unified format
     */
    core::UnifiedSolution solve();

    /**
     * Solve with warm start from previous solution
     * @param warmStart Previous solution to use as starting point
     * @param targetMethod Method to use for improvement
     * @return Improved solution
     */
    core::UnifiedSolution solveWithWarmStart(const core::UnifiedSolution& warmStart,
                                             const std::string&           targetMethod = "exact");

  private:
    std::unique_ptr<data::DataLoader> dataLoader_;
    bool                              problemLoaded_;

    // Solving methods
    core::UnifiedSolution solveGreedy(const SolverConfig& config);
    core::UnifiedSolution solveExact(const SolverConfig& config);
    core::UnifiedSolution solveMeta(const SolverConfig& config);

    // Utility
    void logMessage(const std::string& message, bool verbose = false) const;
};
