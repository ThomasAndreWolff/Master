#pragma once

#include "core/UnifiedSolution.hpp"
#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>
#include <vector>

namespace core {

/**
 * Modern database manager for unified solution format
 */
class UnifiedSolutionDatabase {
  public:
    /**
     * Constructor
     * @param dbPath Path to SQLite database file
     * @param verbose Enable verbose logging
     */
    UnifiedSolutionDatabase(const std::string& dbPath, bool verbose = false);

    /**
     * Destructor
     */
    ~UnifiedSolutionDatabase();

    /**
     * Initialize database connection and create tables
     */
    bool initialize();

    /**
     * Save unified solution to database
     * @param solution The solution to save
     * @return Solution ID if successful, -1 if failed
     */
    int saveSolution(const UnifiedSolution& solution);

    /**
     * Load solution by ID
     * @param solutionId Database ID of solution
     * @return Unified solution or empty solution if not found
     */
    UnifiedSolution loadSolution(int solutionId);

    /**
     * Load all solutions for specific algorithm
     * @param algorithmType Algorithm type ("greedy", "exact", etc.)
     * @return Vector of solutions
     */
    std::vector<UnifiedSolution> loadSolutionsByAlgorithm(const std::string& algorithmType);

    /**
     * Get best solution by objective value
     * @param algorithmType Optional algorithm filter
     * @return Best solution found
     */
    UnifiedSolution getBestSolution(const std::string& algorithmType = "");

    /**
     * Get solution ranking by objective value
     * @param algorithmType Optional algorithm filter
     * @param limit Maximum number of solutions to return
     * @return Vector of solutions ranked by objective
     */
    std::vector<UnifiedSolution> getSolutionRanking(const std::string& algorithmType = "",
                                                    int                limit         = 10);

    /**
     * Clear all solutions for specific algorithm
     * @param algorithmType Algorithm to clear
     * @return true if successful
     */
    bool clearAlgorithm(const std::string& algorithmType);

    /**
     * Check if database is initialized
     */
    bool isInitialized() const { return initialized_; }

  private:
    std::string dbPath_;
    bool        verbose_;
    bool        initialized_;

    std::unique_ptr<SQLite::Database> db_;

    // Prepared statements
    std::unique_ptr<SQLite::Statement> insertSolutionStmt_;
    std::unique_ptr<SQLite::Statement> insertBlockStmt_;

    /**
     * Prepare SQL statements
     */
    bool prepareStatements();

    /**
     * Create database tables
     */
    void createTables();

    /**
     * Save vehicle blocks for a solution
     */
    bool saveVehicleBlocks(int solutionId, const std::vector<VehicleBlock>& blocks);

    /**
     * Load vehicle blocks for a solution
     */
    std::vector<VehicleBlock> loadVehicleBlocks(int solutionId);

    /**
     * Log helper methods
     */
    void logInfo(const std::string& message) const;
    void logError(const std::string& message) const;
};

} // namespace core
