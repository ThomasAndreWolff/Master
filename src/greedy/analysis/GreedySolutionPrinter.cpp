#include "greedy/GreedySolutionPrinter.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_set>

namespace greedy {

GreedySolutionPrinter::GreedySolutionPrinter(const data::DataLoader& dataLoader)
  : dataLoader_(dataLoader) {}

void GreedySolutionPrinter::printSolution(const data::MDVSPGreedySolution& solution,
                                          std::ostream&                    out) const {
    out << "=== Greedy MDVSP Solution ===" << std::endl;
    out << std::endl;

    printSummary(solution, out);
    out << std::endl;

    if (!solution.blocks.empty()) {
        out << "=== Vehicle Blocks ===" << std::endl;
        for (int i = 0; i < solution.blocks.size(); ++i) {
            printBlock(solution.blocks[i], i + 1, out);
            out << std::endl;
        }
    }
}

void GreedySolutionPrinter::printSummary(const data::MDVSPGreedySolution& solution,
                                         std::ostream&                    out) const {
    out << "Solution Summary:" << std::endl;
    out << "  Total Objective Cost: " << formatCost(solution.totalObjectiveCost) << std::endl;
    out << "  Total Vehicles Used:  " << solution.totalVehiclesUsed << std::endl;
    out << "  Feasible Solution:    " << (solution.isFeasible ? "YES" : "NO") << std::endl;
    out << "  Solution Time:        " << std::fixed << std::setprecision(3) << solution.solutionTime
        << " seconds" << std::endl;

    if (!solution.blocks.empty()) {
        // Calculate additional statistics
        double                       totalDistance = 0.0;
        int                          totalTime     = 0;
        int                          totalJourneys = 0;
        std::unordered_map<int, int> vehicleTypeUsage;
        std::unordered_map<int, int> depotUsage;

        for (const auto& block : solution.blocks) {
            totalDistance += block.totalDistance;
            totalTime += block.totalTime;
            totalJourneys += block.serviceJourneyIds.size();
            vehicleTypeUsage[block.vehicleTypeId]++;
            depotUsage[block.depotStopId]++;
        }

        out << std::endl;
        out << "Operation Statistics:" << std::endl;
        out << "  Total Distance:       " << std::fixed << std::setprecision(1)
            << totalDistance / 1000.0 << " km" << std::endl;
        out << "  Total Operating Time: " << secondsToTimeString(totalTime) << std::endl;
        out << "  Total Service Journeys: " << totalJourneys << std::endl;
        out << "  Average Journeys/Vehicle: " << std::fixed << std::setprecision(1)
            << (solution.totalVehiclesUsed > 0 ? (double) totalJourneys / solution.totalVehiclesUsed
                                               : 0.0)
            << std::endl;

        out << std::endl;
        out << "Vehicle Type Usage:" << std::endl;
        for (const auto& usage : vehicleTypeUsage) {
            const auto* vehType = dataLoader_.getVehicleType(usage.first);
            if (vehType) {
                out << "  " << vehType->code << " (" << vehType->name << "): " << usage.second
                    << " vehicles" << std::endl;
            }
        }

        out << std::endl;
        out << "Depot Usage:" << std::endl;
        for (const auto& usage : depotUsage) {
            const auto* depot = dataLoader_.getStopPoint(usage.first);
            if (depot) {
                out << "  " << depot->code << ": " << usage.second << " vehicles" << std::endl;
            }
        }
    }
}

void GreedySolutionPrinter::printBlock(const data::VehicleBlock& block,
                                       int                       blockId,
                                       std::ostream&             out) const {
    out << "Block " << blockId << ":" << std::endl;

    const auto* vehType = dataLoader_.getVehicleType(block.vehicleTypeId);
    const auto* depot   = dataLoader_.getStopPoint(block.depotStopId);

    out << "  Vehicle Type: " << (vehType ? vehType->code : "Unknown") << std::endl;
    out << "  Depot: " << (depot ? depot->code : "Unknown") << std::endl;
    out << "  Total Cost: " << formatCost(block.totalCost) << std::endl;
    out << "  Total Distance: " << std::fixed << std::setprecision(1)
        << block.totalDistance / 1000.0 << " km" << std::endl;
    out << "  Total Time: " << secondsToTimeString(block.totalTime) << std::endl;
    out << "  Service Journeys: " << block.serviceJourneyIds.size() << std::endl;

    if (!block.serviceJourneyIds.empty()) {
        out << "  Journey Schedule:" << std::endl;

        for (int i = 0; i < block.serviceJourneyIds.size(); ++i) {
            int         journeyIdx = block.serviceJourneyIds[i];
            const auto& journey    = dataLoader_.getServiceJourneys()[journeyIdx];

            const auto* fromStop = dataLoader_.getStopPoint(journey.fromStopId);
            const auto* toStop   = dataLoader_.getStopPoint(journey.toStopId);

            out << "    " << (i + 1) << ". Journey " << journey.id << " (Line " << journey.lineId
                << ")" << std::endl;
            out << "       " << (fromStop ? fromStop->code : "???") << " -> "
                << (toStop ? toStop->code : "???") << std::endl;
            out << "       " << secondsToTimeString(journey.depTime) << " - "
                << secondsToTimeString(journey.arrTime) << std::endl;
            out << "       Distance: " << std::fixed << std::setprecision(1)
                << journey.distance / 1000.0 << " km" << std::endl;
        }
    }
}

bool GreedySolutionPrinter::printToFile(const data::MDVSPGreedySolution& solution,
                                        const std::string&               filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    printSolution(solution, file);
    return true;
}

bool GreedySolutionPrinter::validateSolution(const data::MDVSPGreedySolution& solution,
                                             std::ostream&                    out) const {
    out << "=== Solution Validation ===" << std::endl;

    bool                    isValid = true;
    std::unordered_set<int> coveredJourneys;

    // Check if all journeys are covered
    for (const auto& block : solution.blocks) {
        for (int journeyIdx : block.serviceJourneyIds) {
            if (coveredJourneys.count(journeyIdx)) {
                out << "ERROR: Journey " << journeyIdx << " is covered by multiple blocks!"
                    << std::endl;
                isValid = false;
            }
            coveredJourneys.insert(journeyIdx);
        }
    }

    const auto& allJourneys   = dataLoader_.getServiceJourneys();
    int         totalJourneys = allJourneys.size();
    int         coveredCount  = coveredJourneys.size();

    out << "Journey Coverage: " << coveredCount << " / " << totalJourneys << std::endl;

    if (coveredCount < totalJourneys) {
        out << "ERROR: " << (totalJourneys - coveredCount) << " journeys are not covered!"
            << std::endl;
        isValid = false;
    }

    // Check time constraints within blocks
    for (int blockIdx = 0; blockIdx < solution.blocks.size(); ++blockIdx) {
        const auto& block = solution.blocks[blockIdx];

        for (int i = 1; i < block.serviceJourneyIds.size(); ++i) {
            int prevJourneyIdx = block.serviceJourneyIds[i - 1];
            int currJourneyIdx = block.serviceJourneyIds[i];

            const auto& prevJourney = allJourneys[prevJourneyIdx];
            const auto& currJourney = allJourneys[currJourneyIdx];

            int deadHeadTime =
                dataLoader_.getDeadRunTime(prevJourney.toStopId, currJourney.fromStopId);
            if (deadHeadTime < 0)
                deadHeadTime = 0;

            int earliestStart = prevJourney.arrTime + prevJourney.minLayoverTime + deadHeadTime;

            if (earliestStart > currJourney.depTime) {
                out << "ERROR: Time constraint violation in block " << (blockIdx + 1)
                    << " between journeys " << prevJourney.id << " and " << currJourney.id
                    << std::endl;
                isValid = false;
            }
        }
    }

    // Check vehicle type compatibility
    for (int blockIdx = 0; blockIdx < solution.blocks.size(); ++blockIdx) {
        const auto& block = solution.blocks[blockIdx];

        for (int journeyIdx : block.serviceJourneyIds) {
            const auto& journey = allJourneys[journeyIdx];

            if (!dataLoader_.isVehicleTypeCompatible(block.vehicleTypeId, journey.vehTypeGroupId)) {
                out << "ERROR: Vehicle type " << block.vehicleTypeId
                    << " is not compatible with journey " << journey.id << " (requires group "
                    << journey.vehTypeGroupId << ")" << std::endl;
                isValid = false;
            }
        }
    }

    out << "Solution is " << (isValid ? "VALID" : "INVALID") << std::endl;
    return isValid;
}

std::string GreedySolutionPrinter::secondsToTimeString(int seconds) const {
    int hours   = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs    = seconds % 60;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":" << std::setfill('0') << std::setw(2)
        << minutes << ":" << std::setfill('0') << std::setw(2) << secs;

    return oss.str();
}

std::string GreedySolutionPrinter::formatCost(double cost) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << cost << " €";
    return oss.str();
}

} // namespace greedy
