#pragma once

#include <string>
#include <vector>

namespace core {

/**
 * Unified vehicle block representation for all algorithms
 */
struct VehicleBlock {
    int              blockNumber;   // Unique block ID within solution
    int              depotId;       // Depot where vehicle starts/ends
    int              vehicleTypeId; // Type of vehicle used
    std::vector<int> journeyIds;    // Service journeys in order
    double           totalCost;     // Total cost of this block
    double           totalDistance; // Total distance (optional)
    int              totalTime;     // Total time in seconds (optional)
};

/**
 * Unified solution format for all MDVSP algorithms
 */
class UnifiedSolution {
  public:
    // Solution metadata
    std::string algorithmType;       // "greedy", "exact", "local_search", etc.
    double      objectiveValue;      // Total objective function value
    double      solutionTimeSeconds; // Time to find solution
    std::string status;              // "FEASIBLE", "OPTIMAL", "INFEASIBLE"
    std::string problemInstance;     // Problem instance name
    std::string timestamp;           // When solution was found

    // Solution data
    std::vector<VehicleBlock> blocks; // Vehicle blocks (core logic)

    // Constructors
    UnifiedSolution();
    UnifiedSolution(const std::string& algType,
                    double             objective,
                    double             solutionTime,
                    const std::string& solutionStatus);

    // Utility methods
    int  getNumVehicles() const { return static_cast<int>(blocks.size()); }
    int  getNumJourneys() const;
    bool isFeasible() const { return status == "FEASIBLE" || status == "OPTIMAL"; }

  private:
    std::string generateTimestamp() const;
};

} // namespace core
