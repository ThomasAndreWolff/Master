#include "greedy/GreedyMDVSPSolver.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <numeric>

namespace greedy {

GreedyMDVSPSolver::GreedyMDVSPSolver(const data::DataLoader& dataLoader)
  : dataLoader_(dataLoader), verbose_(false) {}

data::MDVSPGreedySolution GreedyMDVSPSolver::solve() {
    auto startTime = std::chrono::high_resolution_clock::now();

    log("Starting Greedy MDVSP Solver...");

    data::MDVSPGreedySolution solution;
    initialize();

    // Get journeys sorted by departure time
    auto sortedJourneys = getSortedJourneyIndices();

    log("Processing " + std::to_string(sortedJourneys.size()) + " service journeys...");

    // Greedy assignment
    for (int journeyIdx : sortedJourneys) {
        const auto& journey = dataLoader_.getServiceJourneys()[journeyIdx];

        // Try to assign to existing block first (cheaper)
        if (!tryAssignToExistingBlock(journeyIdx)) {
            // Create new block if necessary
            if (!createNewBlock(journeyIdx)) {
                log("WARNING: Could not assign journey " + std::to_string(journey.id));
                solution.isFeasible = false;
            }
        }

        assignedJourneys_.insert(journeyIdx);
    }

    // Calculate solution metrics
    solution.blocks             = currentBlocks_;
    solution.totalObjectiveCost = calculateTotalCost();
    solution.totalVehiclesUsed  = currentBlocks_.size();
    solution.isFeasible = (assignedJourneys_.size() == dataLoader_.getServiceJourneys().size());

    auto endTime  = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    solution.solutionTime = duration.count() / 1000.0;

    log("Greedy solution completed:");
    log("  - Total cost: " + std::to_string(solution.totalObjectiveCost));
    log("  - Vehicles used: " + std::to_string(solution.totalVehiclesUsed));
    log("  - Feasible: " + std::string(solution.isFeasible ? "YES" : "NO"));
    log("  - Solution time: " + std::to_string(solution.solutionTime) + " seconds");

    return solution;
}

void GreedyMDVSPSolver::initialize() {
    currentBlocks_.clear();
    assignedJourneys_.clear();
    depotVehicleCount_.clear();
}

std::vector<int> GreedyMDVSPSolver::getSortedJourneyIndices() const {
    const auto& journeys = dataLoader_.getServiceJourneys();

    std::vector<int> indices(journeys.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Sort by departure time
    std::sort(indices.begin(), indices.end(), [&journeys](int a, int b) {
        return journeys[a].depTime < journeys[b].depTime;
    });

    return indices;
}

bool GreedyMDVSPSolver::tryAssignToExistingBlock(int journeyIdx) {
    const auto& journey = dataLoader_.getServiceJourneys()[journeyIdx];

    double bestCost     = std::numeric_limits<double>::max();
    int    bestBlockIdx = -1;

    // Check all existing blocks
    for (int i = 0; i < currentBlocks_.size(); ++i) {
        auto& block = currentBlocks_[i];

        if (canAppendJourney(block, journey)) {
            double cost = calculateAppendCost(block, journey);
            if (cost < bestCost) {
                bestCost     = cost;
                bestBlockIdx = i;
            }
        }
    }

    // Assign to best block if found
    if (bestBlockIdx != -1) {
        appendJourneyToBlock(currentBlocks_[bestBlockIdx], journey);
        return true;
    }

    return false;
}

bool GreedyMDVSPSolver::createNewBlock(int journeyIdx) {
    const auto& journey = dataLoader_.getServiceJourneys()[journeyIdx];

    // Find best vehicle type for this journey
    int vehicleTypeId = findBestVehicleType(journey);
    if (vehicleTypeId == -1) {
        return false;
    }

    // Find best depot
    int depotId = findBestDepot(journey, vehicleTypeId);
    if (depotId == -1) {
        return false;
    }

    // Check depot capacity
    if (!hasDepotCapacity(depotId, vehicleTypeId)) {
        return false;
    }

    // Create new block
    data::VehicleBlock newBlock;
    newBlock.vehicleTypeId = vehicleTypeId;
    newBlock.depotStopId   = depotId;
    newBlock.serviceJourneyIds.push_back(journeyIdx);

    // Calculate initial costs
    const auto* vehicleType = dataLoader_.getVehicleType(vehicleTypeId);
    if (vehicleType) {
        newBlock.totalCost = vehicleType->vehicleCost;

        // Add dead head cost from depot to first journey
        int    deadHeadTime     = dataLoader_.getDeadRunTime(depotId, journey.fromStopId);
        double deadHeadDistance = dataLoader_.getDeadRunDistance(depotId, journey.fromStopId);

        if (deadHeadTime > 0 && deadHeadDistance > 0) {
            newBlock.totalCost += vehicleType->kmCost * deadHeadDistance / 100.0; // km cost
            newBlock.totalCost += vehicleType->hourCost * deadHeadTime / 3600.0;  // hour cost
            newBlock.totalDistance += deadHeadDistance;
            newBlock.totalTime += deadHeadTime;
        }

        // Add service journey costs
        newBlock.totalCost += vehicleType->kmCost * journey.distance / 100.0;
        newBlock.totalCost += vehicleType->hourCost * (journey.arrTime - journey.depTime) / 3600.0;
        newBlock.totalDistance += journey.distance;
        newBlock.totalTime += (journey.arrTime - journey.depTime);

        // Add return to depot cost
        int    returnTime     = dataLoader_.getDeadRunTime(journey.toStopId, depotId);
        double returnDistance = dataLoader_.getDeadRunDistance(journey.toStopId, depotId);

        if (returnTime > 0 && returnDistance > 0) {
            newBlock.totalCost += vehicleType->kmCost * returnDistance / 100.0;
            newBlock.totalCost += vehicleType->hourCost * returnTime / 3600.0;
            newBlock.totalDistance += returnDistance;
            newBlock.totalTime += returnTime;
        }
    }

    currentBlocks_.push_back(newBlock);

    // Update depot capacity tracking
    depotVehicleCount_[depotId][vehicleTypeId]++;

    return true;
}

bool GreedyMDVSPSolver::canAppendJourney(const data::VehicleBlock&   block,
                                         const data::ServiceJourney& journey) const {
    if (block.serviceJourneyIds.empty())
        return false;

    // Check vehicle type compatibility
    if (!dataLoader_.isVehicleTypeCompatible(block.vehicleTypeId, journey.vehTypeGroupId)) {
        return false;
    }

    // Check time constraints
    int         lastJourneyIdx = block.serviceJourneyIds.back();
    const auto& lastJourney    = dataLoader_.getServiceJourneys()[lastJourneyIdx];

    int deadHeadTime = dataLoader_.getDeadRunTime(lastJourney.toStopId, journey.fromStopId);
    if (deadHeadTime < 0)
        deadHeadTime = 0; // Assume no connection if not found

    int earliestStart = lastJourney.arrTime + lastJourney.minLayoverTime + deadHeadTime;

    return earliestStart <= journey.depTime;
}

double GreedyMDVSPSolver::calculateAppendCost(const data::VehicleBlock&   block,
                                              const data::ServiceJourney& journey) const {
    const auto* vehicleType = dataLoader_.getVehicleType(block.vehicleTypeId);
    if (!vehicleType)
        return std::numeric_limits<double>::max();

    double cost = 0.0;

    // Get last journey
    int         lastJourneyIdx = block.serviceJourneyIds.back();
    const auto& lastJourney    = dataLoader_.getServiceJourneys()[lastJourneyIdx];

    // Dead head cost
    double deadHeadDistance =
        dataLoader_.getDeadRunDistance(lastJourney.toStopId, journey.fromStopId);
    int deadHeadTime = dataLoader_.getDeadRunTime(lastJourney.toStopId, journey.fromStopId);

    if (deadHeadDistance > 0 && deadHeadTime > 0) {
        cost += vehicleType->kmCost * deadHeadDistance / 100.0;
        cost += vehicleType->hourCost * deadHeadTime / 3600.0;
    }

    // Service journey cost
    cost += vehicleType->kmCost * journey.distance / 100.0;
    cost += vehicleType->hourCost * (journey.arrTime - journey.depTime) / 3600.0;

    return cost;
}

void GreedyMDVSPSolver::appendJourneyToBlock(data::VehicleBlock&         block,
                                             const data::ServiceJourney& journey) {
    const auto* vehicleType = dataLoader_.getVehicleType(block.vehicleTypeId);
    if (!vehicleType)
        return;

    // Get last journey for dead head calculation
    int         lastJourneyIdx = block.serviceJourneyIds.back();
    const auto& lastJourney    = dataLoader_.getServiceJourneys()[lastJourneyIdx];

    // Add dead head costs
    double deadHeadDistance =
        dataLoader_.getDeadRunDistance(lastJourney.toStopId, journey.fromStopId);
    int deadHeadTime = dataLoader_.getDeadRunTime(lastJourney.toStopId, journey.fromStopId);

    if (deadHeadDistance > 0 && deadHeadTime > 0) {
        block.totalCost += vehicleType->kmCost * deadHeadDistance / 100.0;
        block.totalCost += vehicleType->hourCost * deadHeadTime / 3600.0;
        block.totalDistance += deadHeadDistance;
        block.totalTime += deadHeadTime;
    }

    // Add service journey costs
    block.totalCost += vehicleType->kmCost * journey.distance / 100.0;
    block.totalCost += vehicleType->hourCost * (journey.arrTime - journey.depTime) / 3600.0;
    block.totalDistance += journey.distance;
    block.totalTime += (journey.arrTime - journey.depTime);

    // Add journey to block
    int journeyIdx = &journey - &dataLoader_.getServiceJourneys()[0];
    block.serviceJourneyIds.push_back(journeyIdx);
}

int GreedyMDVSPSolver::findBestDepot(const data::ServiceJourney& journey, int vehicleTypeId) const {
    auto depots = dataLoader_.getDepotStops();

    double bestCost  = std::numeric_limits<double>::max();
    int    bestDepot = -1;

    const auto* vehicleType = dataLoader_.getVehicleType(vehicleTypeId);
    if (!vehicleType)
        return -1;

    for (int depotId : depots) {
        if (!hasDepotCapacity(depotId, vehicleTypeId))
            continue;

        // Calculate cost from depot to journey start
        double distance = dataLoader_.getDeadRunDistance(depotId, journey.fromStopId);
        int    time     = dataLoader_.getDeadRunTime(depotId, journey.fromStopId);

        if (distance >= 0 && time >= 0) {
            double cost =
                vehicleType->kmCost * distance / 100.0 + vehicleType->hourCost * time / 3600.0;
            if (cost < bestCost) {
                bestCost  = cost;
                bestDepot = depotId;
            }
        }
    }

    return bestDepot;
}

bool GreedyMDVSPSolver::hasDepotCapacity(int depotId, int vehicleTypeId) const {
    int maxCapacity = dataLoader_.getVehicleCapacityAtStop(vehicleTypeId, depotId);

    if (maxCapacity <= 0) {
        return false;
    }

    // Check current usage
    auto depotIt = depotVehicleCount_.find(depotId);
    if (depotIt != depotVehicleCount_.end()) {
        auto vehTypeIt = depotIt->second.find(vehicleTypeId);
        if (vehTypeIt != depotIt->second.end()) {
            return vehTypeIt->second < maxCapacity;
        }
    }

    return true;
}

double GreedyMDVSPSolver::calculateTotalCost() const {
    double totalCost = 0.0;
    for (const auto& block : currentBlocks_) {
        totalCost += calculateBlockCost(block);
    }
    return totalCost;
}

double GreedyMDVSPSolver::calculateBlockCost(const data::VehicleBlock& block) const {
    return block.totalCost;
}

int GreedyMDVSPSolver::findBestVehicleType(const data::ServiceJourney& journey) const {
    auto compatibleTypes = dataLoader_.getCompatibleVehicleTypes(journey.vehTypeGroupId);

    if (compatibleTypes.empty())
        return -1;

    // For now, choose the vehicle type with lowest total cost
    // In a more sophisticated version, we could consider other factors
    double bestCost = std::numeric_limits<double>::max();
    int    bestType = -1;

    for (int vehTypeId : compatibleTypes) {
        const auto* vehType = dataLoader_.getVehicleType(vehTypeId);
        if (vehType) {
            // Simple cost heuristic: combine vehicle cost with operational costs
            double cost = vehType->vehicleCost / 1000.0 + vehType->kmCost + vehType->hourCost;
            if (cost < bestCost) {
                bestCost = cost;
                bestType = vehTypeId;
            }
        }
    }

    return bestType;
}

void GreedyMDVSPSolver::log(const std::string& message) const {
    if (verbose_) {
        std::cout << "[GreedyMDVSPSolver] " << message << std::endl;
    }
}

} // namespace greedy
