#include "core/UnifiedSolutionDatabase.hpp"
#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <sstream>

namespace core {

UnifiedSolutionDatabase::UnifiedSolutionDatabase(const std::string& dbPath, bool verbose)
  : dbPath_(dbPath), verbose_(verbose), initialized_(false) {}

UnifiedSolutionDatabase::~UnifiedSolutionDatabase() {
    // SQLiteCpp handles cleanup automatically with unique_ptr
}

bool UnifiedSolutionDatabase::initialize() {
    try {
        logInfo("Initializing database connection...");

        // Create database using unique_ptr
        db_ = std::make_unique<SQLite::Database>(dbPath_,
                                                 SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

        // Create tables
        createTables();

        // Prepare statements
        if (!prepareStatements()) {
            logError("Failed to prepare statements");
            return false;
        }

        initialized_ = true;
        logInfo("Database initialized successfully: " + dbPath_);
        return true;
    }
    catch (const std::exception& e) {
        logError("Database initialization failed: " + std::string(e.what()));
        return false;
    }
}

void UnifiedSolutionDatabase::createTables() {
    const char* createSolutionsTable = R"(
        CREATE TABLE IF NOT EXISTS unified_solutions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            algorithm_type TEXT NOT NULL,
            objective_value REAL NOT NULL,
            solution_time_seconds REAL NOT NULL,
            status TEXT NOT NULL,
            problem_instance TEXT NOT NULL,
            timestamp TEXT NOT NULL,
            num_vehicles INTEGER NOT NULL,
            num_journeys INTEGER NOT NULL
        );
    )";

    const char* createBlocksTable = R"(
        CREATE TABLE IF NOT EXISTS unified_vehicle_blocks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            solution_id INTEGER NOT NULL,
            block_number INTEGER NOT NULL,
            depot_id INTEGER NOT NULL,
            vehicle_type_id INTEGER NOT NULL,
            total_cost REAL NOT NULL,
            total_distance REAL,
            total_time INTEGER,
            journey_ids TEXT NOT NULL,
            FOREIGN KEY (solution_id) REFERENCES unified_solutions(id)
        );
    )";

    db_->exec(createSolutionsTable);
    db_->exec(createBlocksTable);
}

bool UnifiedSolutionDatabase::prepareStatements() {
    try {
        insertSolutionStmt_ = std::make_unique<SQLite::Statement>(*db_, R"(
            INSERT INTO unified_solutions 
            (algorithm_type, objective_value, solution_time_seconds, status, 
             problem_instance, timestamp, num_vehicles, num_journeys)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        )");

        insertBlockStmt_ = std::make_unique<SQLite::Statement>(*db_, R"(
            INSERT INTO unified_vehicle_blocks 
            (solution_id, block_number, depot_id, vehicle_type_id, 
             total_cost, total_distance, total_time, journey_ids)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        )");

        return true;
    }
    catch (const std::exception& e) {
        logError("Failed to prepare statements: " + std::string(e.what()));
        return false;
    }
}

int UnifiedSolutionDatabase::saveSolution(const UnifiedSolution& solution) {
    if (!initialized_) {
        logError("Database not initialized");
        return -1;
    }

    try {
        logInfo("Saving unified solution to database...");

        // Start transaction
        SQLite::Transaction transaction(*db_);

        // Bind and execute solution insert
        insertSolutionStmt_->bind(1, solution.algorithmType);
        insertSolutionStmt_->bind(2, solution.objectiveValue);
        insertSolutionStmt_->bind(3, solution.solutionTimeSeconds);
        insertSolutionStmt_->bind(4, solution.status);
        insertSolutionStmt_->bind(5, solution.problemInstance);
        insertSolutionStmt_->bind(6, solution.timestamp);
        insertSolutionStmt_->bind(7, solution.getNumVehicles());
        insertSolutionStmt_->bind(8, solution.getNumJourneys());

        insertSolutionStmt_->exec();
        int64_t solutionId = db_->getLastInsertRowid();

        // Save vehicle blocks
        if (!saveVehicleBlocks(static_cast<int>(solutionId), solution.blocks)) {
            logError("Failed to save vehicle blocks");
            return -1;
        }

        // Commit transaction
        transaction.commit();
        insertSolutionStmt_->reset();

        logInfo("Solution saved successfully with ID: " + std::to_string(solutionId));
        return static_cast<int>(solutionId);
    }
    catch (const std::exception& e) {
        logError("Failed to save solution: " + std::string(e.what()));
        return -1;
    }
}

bool UnifiedSolutionDatabase::saveVehicleBlocks(int                              solutionId,
                                                const std::vector<VehicleBlock>& blocks) {
    try {
        for (const auto& block : blocks) {
            insertBlockStmt_->bind(1, solutionId);
            insertBlockStmt_->bind(2, block.blockNumber);
            insertBlockStmt_->bind(3, block.depotId);
            insertBlockStmt_->bind(4, block.vehicleTypeId);
            insertBlockStmt_->bind(5, block.totalCost);
            insertBlockStmt_->bind(6, block.totalDistance);
            insertBlockStmt_->bind(7, block.totalTime);

            // Convert journey IDs to comma-separated string
            std::ostringstream journeyStream;
            for (size_t i = 0; i < block.journeyIds.size(); ++i) {
                if (i > 0)
                    journeyStream << ",";
                journeyStream << block.journeyIds[i];
            }
            insertBlockStmt_->bind(8, journeyStream.str());

            insertBlockStmt_->exec();
            insertBlockStmt_->reset();
        }
        return true;
    }
    catch (const std::exception& e) {
        logError("Failed to save vehicle blocks: " + std::string(e.what()));
        return false;
    }
}

UnifiedSolution UnifiedSolutionDatabase::loadSolution(int solutionId) {
    // TODO: Implement solution loading by ID
    return UnifiedSolution();
}

std::vector<UnifiedSolution>
UnifiedSolutionDatabase::loadSolutionsByAlgorithm(const std::string& algorithmType) {
    // TODO: Implement loading solutions by algorithm
    return std::vector<UnifiedSolution>();
}

UnifiedSolution UnifiedSolutionDatabase::getBestSolution(const std::string& algorithmType) {
    // TODO: Implement best solution retrieval
    return UnifiedSolution();
}

std::vector<UnifiedSolution>
UnifiedSolutionDatabase::getSolutionRanking(const std::string& algorithmType, int limit) {
    // TODO: Implement solution ranking
    return std::vector<UnifiedSolution>();
}

bool UnifiedSolutionDatabase::clearAlgorithm(const std::string& algorithmType) {
    // TODO: Implement algorithm clearing
    return false;
}

std::vector<VehicleBlock> UnifiedSolutionDatabase::loadVehicleBlocks(int solutionId) {
    // TODO: Implement vehicle block loading
    return std::vector<VehicleBlock>();
}

void UnifiedSolutionDatabase::logInfo(const std::string& message) const {
    if (verbose_) {
        std::cout << "[UnifiedDB] " << message << std::endl;
    }
}

void UnifiedSolutionDatabase::logError(const std::string& message) const {
    std::cerr << "[UnifiedDB] ERROR: " << message << std::endl;
}

} // namespace core
