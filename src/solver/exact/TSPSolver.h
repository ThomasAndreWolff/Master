#ifndef TSP_SOLVER_H
#define TSP_SOLVER_H

#include <vector>
#include <utility>
#include "gurobi_c++.h"

class TSPSolver
{
private:
    GRBEnv &env;
    const std::vector<std::pair<double, double>> &cities;

    double calculateDistance(const std::pair<double, double> &a, const std::pair<double, double> &b);

public:
    TSPSolver(GRBEnv &env, const std::vector<std::pair<double, double>> &cities);
    void solve();
};

#endif // TSP_SOLVER_H
