#include "core/SolutionConverter.hpp"
#include "core/UnifiedSolution.hpp"
#include "data/DataStructures.hpp"
#include <algorithm>
#include <sstream>

namespace core {

UnifiedSolution SolutionConverter::fromGreedySolution(const data::MDVSPGreedySolution& greedySolution) {
    UnifiedSolution unified("greedy",
                            greedySolution.totalObjectiveCost,
                            greedySolution.solutionTime,
                            greedySolution.isFeasible ? "FEASIBLE" : "INFEASIBLE");

    // Convert vehicle blocks
    for (size_t i = 0; i < greedySolution.blocks.size(); ++i) {
        const auto& greedyBlock = greedySolution.blocks[i];

        VehicleBlock block;
        block.blockNumber   = static_cast<int>(i);
        block.depotId       = greedyBlock.depotStopId;
        block.vehicleTypeId = greedyBlock.vehicleTypeId;
        block.journeyIds    = greedyBlock.serviceJourneyIds;
        block.totalCost     = greedyBlock.totalCost;
        block.totalDistance = greedyBlock.totalDistance;
        block.totalTime     = greedyBlock.totalTime;

        unified.blocks.push_back(block);
    }

    return unified;
}

data::MDVSPGreedySolution SolutionConverter::toGreedySolution(const UnifiedSolution& unified) {
    data::MDVSPGreedySolution greedy;

    greedy.totalObjectiveCost = unified.objectiveValue;
    greedy.solutionTime       = unified.solutionTimeSeconds;
    greedy.isFeasible         = unified.isFeasible();
    greedy.totalVehiclesUsed  = unified.getNumVehicles();

    // Convert vehicle blocks
    for (const auto& block : unified.blocks) {
        data::VehicleBlock greedyBlock;
        greedyBlock.vehicleTypeId     = block.vehicleTypeId;
        greedyBlock.depotStopId       = block.depotId;
        greedyBlock.serviceJourneyIds = block.journeyIds;
        greedyBlock.totalCost         = block.totalCost;
        greedyBlock.totalDistance     = block.totalDistance;
        greedyBlock.totalTime         = block.totalTime;

        greedy.blocks.push_back(greedyBlock);
    }

    return greedy;
}

std::vector<double> SolutionConverter::generateSolverVariables(const UnifiedSolution& solution,
                                                               int                    totalJourneys,
                                                               int totalVehicles) {
    std::vector<double> variables(totalJourneys * totalVehicles, 0.0);

    for (const auto& block : solution.blocks) {
        for (int journeyId : block.journeyIds) {
            if (journeyId >= 0 && journeyId < totalJourneys && block.blockNumber >= 0 &&
                block.blockNumber < totalVehicles) {
                // Variable naming: x[journey][vehicle] = journey * totalVehicles + vehicle
                int varIndex        = journeyId * totalVehicles + block.blockNumber;
                variables[varIndex] = 1.0;
            }
        }
    }

    return variables;
}

std::vector<std::string> SolutionConverter::generateVariableNames(int totalJourneys,
                                                                  int totalVehicles) {
    std::vector<std::string> names;
    names.reserve(totalJourneys * totalVehicles);

    for (int j = 0; j < totalJourneys; ++j) {
        for (int v = 0; v < totalVehicles; ++v) {
            names.push_back("x_journey_" + std::to_string(j) + "_vehicle_" + std::to_string(v));
        }
    }

    return names;
}

std::vector<int> SolutionConverter::parseJourneyIds(const std::string& journeyIdsStr) {
    std::vector<int> journeyIds;
    if (journeyIdsStr.empty()) {
        return journeyIds;
    }

    std::stringstream ss(journeyIdsStr);
    std::string       item;

    while (std::getline(ss, item, ',')) {
        try {
            journeyIds.push_back(std::stoi(item));
        }
        catch (const std::exception&) {
            // Skip invalid entries
        }
    }

    return journeyIds;
}

std::string SolutionConverter::formatJourneyIds(const std::vector<int>& journeyIds) {
    if (journeyIds.empty()) {
        return "";
    }

    std::stringstream ss;
    for (size_t i = 0; i < journeyIds.size(); ++i) {
        if (i > 0)
            ss << ",";
        ss << journeyIds[i];
    }

    return ss.str();
}

} // namespace core
