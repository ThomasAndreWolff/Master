#include "TSPResultLogger.h"
#include <filesystem>
#include <sstream>

TSPResultLogger::TSPResultLogger(const std::string& dbPath) {
    // Ensure directory exists
    std::filesystem::create_directories(std::filesystem::path(dbPath).parent_path());
    db = new SQLite::Database(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    db->exec("CREATE TABLE IF NOT EXISTS best_individuals (generation INTEGER, fitness REAL, tour "
             "TEXT)");
}

TSPResultLogger::~TSPResultLogger() {
    delete db;
}

void TSPResultLogger::logBestIndividual(int generation, double fitness, const std::vector<int>& tour) {
    std::ostringstream tour_stream;
    for (size_t i = 0; i < tour.size(); ++i) {
        tour_stream << tour[i];
        if (i + 1 < tour.size())
            tour_stream << " ";
    }
    SQLite::Statement query(
        *db, "INSERT INTO best_individuals (generation, fitness, tour) VALUES (?, ?, ?)");
    query.bind(1, generation);
    query.bind(2, fitness);
    query.bind(3, tour_stream.str());
    query.exec();
}
