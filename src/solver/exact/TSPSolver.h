#ifndef TSP_SOLVER_H
#define TSP_SOLVER_H

#include <utility>
#include <vector>
#include "gurobi_c++.h"

class TSPSolver {
  private:
    GRBEnv&                                       env;
    const std::vector<std::pair<double, double>>& cities;
    std::vector<int>                              initialSolution; // Store the initial solution

    double calculateDistance(const std::pair<double, double>& a, const std::pair<double, double>& b);

  public:
    TSPSolver(GRBEnv&                                       env,
              const std::vector<std::pair<double, double>>& cities,
              const std::vector<int>&                       initialSolution = {});
    void solve();
};

#endif // TSP_SOLVER_H
