#pragma once

#include "core/UnifiedSolution.hpp"
#include "data/DataStructures.hpp"
#include "data/DataLoader.hpp"
#include <string>

namespace core {

/**
 * Transforms different solution formats to unified format
 * This allows all solvers to output their native format and convert later
 */
class SolutionTransformer {
  public:
    /**
     * Transform greedy solution to unified format
     * @param greedySolution The greedy solution to transform
     * @param dataLoader Data loader for context information
     * @param problemInstance Name of the problem instance
     * @return Unified solution
     */
    static UnifiedSolution transform(const data::MDVSPGreedySolution& greedySolution,
                                   const data::DataLoader& dataLoader,
                                   const std::string& problemInstance = "unknown");

    /**
     * Transform exact solution to unified format (for future use)
     * @param exactSolution The exact solution to transform
     * @param problemInstance Name of the problem instance  
     * @return Unified solution
     */
    // static UnifiedSolution transform(const ExactSolution& exactSolution, const std::string& problemInstance);

  private:
    /**
     * Generate current timestamp string
     */
    static std::string getCurrentTimestamp();

    /**
     * Convert greedy blocks to unified vehicle blocks
     */
    static std::vector<VehicleBlock> convertBlocks(const std::vector<data::VehicleBlock>& greedyBlocks);
};

} // namespace core
