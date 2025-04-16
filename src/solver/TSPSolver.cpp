#include "TSPSolver.h"
#include <cmath>
#include <iostream>

TSPSolver::TSPSolver(GRBEnv &env, const std::vector<std::pair<double, double>> &cities)
    : env(env), cities(cities) {}

double TSPSolver::calculateDistance(const std::pair<double, double> &a, const std::pair<double, double> &b)
{
    return std::sqrt(std::pow(a.first - b.first, 2) + std::pow(a.second - b.second, 2));
}

void TSPSolver::solve()
{
    try
    {
        GRBModel model(env);

        int n = cities.size();
        std::vector<std::vector<GRBVar>> vars(n, std::vector<GRBVar>(n));

        // Create variables
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                if (i != j)
                {
                    vars[i][j] = model.addVar(0.0, 1.0, calculateDistance(cities[i], cities[j]), GRB_BINARY,
                                              "x_" + std::to_string(i) + "_" + std::to_string(j));
                }
            }
        }

        // Add constraints
        for (int i = 0; i < n; ++i)
        {
            GRBLinExpr outFlow = 0, inFlow = 0;
            for (int j = 0; j < n; ++j)
            {
                if (i != j)
                {
                    outFlow += vars[i][j];
                    inFlow += vars[j][i];
                }
            }
            model.addConstr(outFlow == 1, "out_" + std::to_string(i));
            model.addConstr(inFlow == 1, "in_" + std::to_string(i));
        }

        // Optimize model
        model.optimize();

        // Output solution
        // std::cout << "Optimal tour:" << std::endl;
        // for (int i = 0; i < n; ++i)
        // {
        //     for (int j = 0; j < n; ++j)
        //     {
        //         if (i != j && vars[i][j].get(GRB_DoubleAttr_X) > 0.5)
        //         {
        //             std::cout << i << " -> " << j << std::endl;
        //         }
        //     }
        // }
    }
    catch (GRBException &e)
    {
        std::cout << "Gurobi error: " << e.getMessage() << std::endl;
    }
}