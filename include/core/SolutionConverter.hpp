#pragma once

#include "core/UnifiedSolution.hpp"
#include "data/DataStructures.hpp"
#include <string>
#include <vector>

namespace core {

/**
 * Converts between different solution formats
 */
class SolutionConverter {
  public:
    /**
     * Convert greedy solution to unified format
     */
    static UnifiedSolution fromGreedySolution(const data::MDVSPGreedySolution& greedySolution);

    /**
     * Convert unified solution back to greedy format
     */
    static data::MDVSPGreedySolution toGreedySolution(const UnifiedSolution& unified);

    /**
     * Generate solver variables from unified solution for warm start
     */
    static std::vector<double>
    generateSolverVariables(const UnifiedSolution& solution, int totalJourneys, int totalVehicles);

    /**
     * Generate variable names for the solver
     */
    static std::vector<std::string> generateVariableNames(int totalJourneys, int totalVehicles);

    /**
     * Parse journey IDs from string representation
     */
    static std::vector<int> parseJourneyIds(const std::string& journeyIdsStr);

    /**
     * Format journey IDs to string representation
     */
    static std::string formatJourneyIds(const std::vector<int>& journeyIds);
};

} // namespace core
