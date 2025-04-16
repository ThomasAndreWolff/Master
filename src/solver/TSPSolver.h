#ifndef TSPSOLVER_H
#define TSPSOLVER_H

#include "gurobi_c++.h"
#include <vector>
#include <utility>

class TSPSolver
{
public:
    TSPSolver(GRBEnv &env, const std::vector<std::pair<double, double>> &cities);
    void solve();

private:
    GRBEnv &env;
    std::vector<std::pair<double, double>> cities;

    double calculateDistance(const std::pair<double, double> &a, const std::pair<double, double> &b);
};

#endif // TSPSOLVER_H
