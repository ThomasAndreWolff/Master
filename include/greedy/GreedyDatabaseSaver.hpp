#pragma once

#include "data/DataStructures.hpp"
#include <memory>
#include <string>
#include <vector>

// Forward declarations
namespace SQLite {
class Database;
}

namespace mdvsp {
class MDVSPDatabaseInitializer;
class MDVSPDatabaseWriter;
} // namespace mdvsp

namespace mdvsp {
namespace greedy {

/**
 * Handles saving greedy MDVSP solutions to SQLite database
 */
class GreedyDatabaseSaver {
  public:
    /**
     * Constructor
     * @param dbPath Path to SQLite database file
     * @param runId Unique identifier for this greedy run
     * @param verbose Enable verbose logging
     */
    GreedyDatabaseSaver(const std::string& dbPath,
                        const std::string& runId   = "greedy",
                        bool               verbose = false);

    /**
     * Destructor
     */
    ~GreedyDatabaseSaver();

    /**
     * Initialize database connection and create tables if needed
     * @return true if successful, false otherwise
     */
    bool initialize();

    /**
     * Create database tables
     */
    void createTables();

    /**
     * Save greedy solution to database
     * @param solution The greedy solution to save
     * @return true if successful, false otherwise
     */
    bool saveSolution(const data::MDVSPGreedySolution& solution);

    /**
     * Clear all solutions for the current run ID
     * @return true if successful, false otherwise
     */
    bool clearRun();

    /**
     * Check if database is initialized
     */
    bool isInitialized() const { return initialized_; }

  private:
    std::string dbPath_;
    std::string runId_;
    bool        verbose_;
    bool        initialized_;

    // Database components
    std::unique_ptr<MDVSPDatabaseInitializer> initializer_;
    std::shared_ptr<SQLite::Database>         db_;
    std::unique_ptr<MDVSPDatabaseWriter>      writer_;

    /**
     * Convert greedy solution to database format
     */
    bool saveGreedySolutionData(const data::MDVSPGreedySolution& solution);

    /**
     * Convert greedy solution to variable format for exact solver
     * @param solution The greedy solution
     * @param variableValues Output vector for variable values
     * @param variableNames Output vector for variable names
     * @return true if successful, false otherwise
     */
    bool convertToVariableFormat(const data::MDVSPGreedySolution& solution,
                                 std::vector<double>&             variableValues,
                                 std::vector<std::string>&        variableNames);

    /**
     * Log helper methods
     */
    void logInfo(const std::string& message) const;
    void logError(const std::string& message) const;
};

} // namespace greedy
} // namespace mdvsp
