#pragma once
#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <vector>

class TSPResultLogger {
  public:
    TSPResultLogger(const std::string& dbPath = "database/tsp_results.db");
    ~TSPResultLogger();
    void logBestIndividual(int generation, double fitness, const std::vector<int>& tour);

  private:
    SQLite::Database* db;
};
