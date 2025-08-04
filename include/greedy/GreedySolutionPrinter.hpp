#pragma once

#include "data/DataLoader.hpp"
#include "data/DataStructures.hpp"
#include <ostream>
#include <string>

namespace greedy {

/**
 * Utility class for outputting and analyzing greedy MDVSP solutions
 */
class GreedySolutionPrinter {
  public:
    /**
     * Constructor
     */
    explicit GreedySolutionPrinter(const data::DataLoader& dataLoader);

    /**
     * Print detailed solution to stream
     */
    void printSolution(const data::MDVSPGreedySolution& solution, std::ostream& out) const;

    /**
     * Print solution summary
     */
    void printSummary(const data::MDVSPGreedySolution& solution, std::ostream& out) const;

    /**
     * Print solution to file
     */
    bool printToFile(const data::MDVSPGreedySolution& solution, const std::string& filename) const;

    /**
     * Validate solution feasibility
     */
    bool validateSolution(const data::MDVSPGreedySolution& solution, std::ostream& out) const;

  private:
    const data::DataLoader& dataLoader_;

    /**
     * Print vehicle block details
     */
    void printBlock(const data::VehicleBlock& block, int blockId, std::ostream& out) const;

    /**
     * Convert seconds to time string
     */
    std::string secondsToTimeString(int seconds) const;

    /**
     * Format cost with currency
     */
    std::string formatCost(double cost) const;
};

} // namespace greedy
