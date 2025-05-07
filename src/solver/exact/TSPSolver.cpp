#include "solver/exact/TSPSolver.h"
#include <cmath>
#include <iostream>

TSPSolver::TSPSolver(GRBEnv& env, const std::vector<std::pair<double, double>>& cities)
  : env(env), cities(cities) {}

double TSPSolver::calculateDistance(const std::pair<double, double>& a,
                                    const std::pair<double, double>& b) {
    return std::sqrt(std::pow(a.first - b.first, 2) + std::pow(a.second - b.second, 2));
}

void TSPSolver::setHeuristicSolution(const std::vector<int>& tour) {
    heuristicTour = tour;
}

void TSPSolver::TSPCallback::callback() {
    if (where == GRB_CB_MIPSOL) {
        std::cout << "MIP solution found." << std::endl;
        // Use the heuristic solution if available
        if (!heuristicTour.empty()) {
            int                 n = cities.size();
            std::vector<double> solution(n * n, 0.0);
            std::vector<GRBVar> vars;

            // Populate the solution and vars arrays
            for (size_t i = 0; i < heuristicTour.size(); ++i) {
                int from                = heuristicTour[i];
                int to                  = heuristicTour[(i + 1) % heuristicTour.size()];
                solution[from * n + to] = 1.0;
                vars.push_back(
                    model.getVarByName("x_" + std::to_string(from) + "_" + std::to_string(to)));
            }

            try {
                std::cout << "Setting solution" << std::endl;
                setSolution(vars.data(), solution.data(), vars.size());
            }
            catch (GRBException& e) {
                std::cerr << "Error setting heuristic solution: " << e.getMessage() << std::endl;
            }
        }
    }
}

void TSPSolver::solve() {
    try {
        GRBModel                         model(env);
        int                              n = cities.size();
        std::vector<std::vector<GRBVar>> vars(n, std::vector<GRBVar>(n));

        // Create variables
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i != j) {
                    vars[i][j] = model.addVar(0.0,
                                              1.0,
                                              calculateDistance(cities[i], cities[j]),
                                              GRB_BINARY,
                                              "x_" + std::to_string(i) + "_" + std::to_string(j));
                }
            }
        }

        // Add constraints
        for (int i = 0; i < n; ++i) {
            GRBLinExpr outFlow = 0, inFlow = 0;
            for (int j = 0; j < n; ++j) {
                if (i != j) {
                    outFlow += vars[i][j];
                    inFlow += vars[j][i];
                }
            }
            model.addConstr(outFlow == 1, "out_" + std::to_string(i));
            model.addConstr(inFlow == 1, "in_" + std::to_string(i));
        }

        // Set the callback
        TSPCallback callback(heuristicTour, cities, model);
        model.setCallback(&callback);

        // Optimize model
        model.optimize();
    }
    catch (GRBException& e) {
        std::cout << "Gurobi error: " << e.getMessage() << std::endl;
    }
}