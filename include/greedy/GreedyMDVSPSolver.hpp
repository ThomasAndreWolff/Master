#pragma once

#include "data/DataLoader.hpp"
#include "data/DataStructures.hpp"
#include <unordered_set>
#include <vector>

namespace greedy {

/**
 * Greedy heuristic for solving the Multi-Depot Vehicle Scheduling Problem (MDVSP)
 *
 * This heuristic aims to quickly find a feasible solution by:
 * 1. Sorting service journeys by start time
 * 2. For each journey, finding the best compatible vehicle or creating a new one
 * 3. Minimizing dead head costs and respecting depot capacity constraints
 */
class GreedyMDVSPSolver {
  public:
    /**
     * Constructor
     * @param dataLoader Loaded MDVSP data
     */
    explicit GreedyMDVSPSolver(const data::DataLoader& dataLoader);

    /**
     * Solve the MDVSP using greedy heuristic
     * @return Complete solution
     */
    data::MDVSPGreedySolution solve();

    /**
     * Set verbosity level
     */
    void setVerbose(bool verbose) { verbose_ = verbose; }

  private:
    const data::DataLoader& dataLoader_;
    bool                    verbose_;

    // Current solution state
    std::vector<data::VehicleBlock> currentBlocks_;
    std::unordered_set<int>         assignedJourneys_;

    // Depot capacity tracking
    std::unordered_map<int, std::unordered_map<int, int>> depotVehicleCount_; // depot ->
                                                                              // vehicleType ->
                                                                              // count

    /**
     * Initialize greedy algorithm
     */
    void initialize();

    /**
     * Get sorted service journeys by departure time
     */
    std::vector<int> getSortedJourneyIndices() const;

    /**
     * Try to assign journey to existing vehicle block
     * @param journeyIdx Index of journey in service journeys
     * @return true if successfully assigned, false otherwise
     */
    bool tryAssignToExistingBlock(int journeyIdx);

    /**
     * Create new vehicle block for journey
     * @param journeyIdx Index of journey in service journeys
     * @return true if successfully created, false otherwise
     */
    bool createNewBlock(int journeyIdx);

    /**
     * Check if journey can be appended to block
     */
    bool canAppendJourney(const data::VehicleBlock& block, const data::ServiceJourney& journey) const;

    /**
     * Calculate cost of appending journey to block
     */
    double calculateAppendCost(const data::VehicleBlock&   block,
                               const data::ServiceJourney& journey) const;

    /**
     * Append journey to block
     */
    void appendJourneyToBlock(data::VehicleBlock& block, const data::ServiceJourney& journey);

    /**
     * Find best depot for starting a new block
     */
    int findBestDepot(const data::ServiceJourney& journey, int vehicleTypeId) const;

    /**
     * Check depot capacity constraints
     */
    bool hasDepotCapacity(int depotId, int vehicleTypeId) const;

    /**
     * Calculate total solution cost
     */
    double calculateTotalCost() const;

    /**
     * Calculate block cost (vehicle cost + operational costs)
     */
    double calculateBlockCost(const data::VehicleBlock& block) const;

    /**
     * Get time for last service journey in block
     */
    int getBlockEndTime(const data::VehicleBlock& block) const;

    /**
     * Get end stop for last service journey in block
     */
    int getBlockEndStop(const data::VehicleBlock& block) const;

    /**
     * Find best compatible vehicle type for journey
     */
    int findBestVehicleType(const data::ServiceJourney& journey) const;

    /**
     * Log message if verbose mode is enabled
     */
    void log(const std::string& message) const;
};

} // namespace greedy
