#include "core/SolutionTransformer.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace core {

UnifiedSolution SolutionTransformer::transform(const data::MDVSPGreedySolution& greedySolution,
                                               const data::DataLoader&          dataLoader,
                                               const std::string&               problemInstance) {
    UnifiedSolution unified;

    // Set basic solution information
    unified.algorithmType       = "GREEDY";
    unified.objectiveValue      = greedySolution.totalObjectiveCost;
    unified.solutionTimeSeconds = greedySolution.solutionTime;
    unified.status              = greedySolution.isFeasible ? "FEASIBLE" : "INFEASIBLE";
    unified.problemInstance     = problemInstance;
    unified.timestamp           = getCurrentTimestamp();

    // Convert vehicle blocks
    unified.blocks = convertBlocks(greedySolution.blocks);

    return unified;
}

std::string SolutionTransformer::getCurrentTimestamp() {
    auto              now    = std::chrono::system_clock::now();
    auto              time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::vector<VehicleBlock>
SolutionTransformer::convertBlocks(const std::vector<data::VehicleBlock>& greedyBlocks) {
    std::vector<VehicleBlock> unifiedBlocks;

    for (size_t i = 0; i < greedyBlocks.size(); ++i) {
        const auto& greedyBlock = greedyBlocks[i];

        VehicleBlock unifiedBlock;
        unifiedBlock.blockNumber   = static_cast<int>(i);
        unifiedBlock.depotId       = greedyBlock.depotStopId;
        unifiedBlock.vehicleTypeId = greedyBlock.vehicleTypeId;
        unifiedBlock.totalCost     = greedyBlock.totalCost;
        unifiedBlock.totalDistance = greedyBlock.totalDistance;
        unifiedBlock.totalTime     = greedyBlock.totalTime;
        unifiedBlock.journeyIds    = greedyBlock.serviceJourneyIds;

        unifiedBlocks.push_back(unifiedBlock);
    }

    return unifiedBlocks;
}

} // namespace core
