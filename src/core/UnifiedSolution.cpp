#include "core/UnifiedSolution.hpp"
#include <chrono>
#include <iomanip>
#include <numeric>
#include <sstream>

namespace core {

UnifiedSolution::UnifiedSolution()
  : algorithmType("unknown"),
    objectiveValue(0.0),
    solutionTimeSeconds(0.0),
    status("UNKNOWN"),
    problemInstance("default"),
    timestamp(generateTimestamp()) {}

UnifiedSolution::UnifiedSolution(const std::string& algType,
                                 double             objective,
                                 double             solutionTime,
                                 const std::string& solutionStatus)
  : algorithmType(algType),
    objectiveValue(objective),
    solutionTimeSeconds(solutionTime),
    status(solutionStatus),
    problemInstance("default"),
    timestamp(generateTimestamp()) {}

int UnifiedSolution::getNumJourneys() const {
    return std::accumulate(blocks.begin(), blocks.end(), 0, [](int sum, const VehicleBlock& block) {
        return sum + static_cast<int>(block.journeyIds.size());
    });
}

std::string UnifiedSolution::generateTimestamp() const {
    auto now    = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace core
