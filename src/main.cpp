#include "reader/TSPLibReader.h"
#include "solver/exact/TSPSolver.h"
#include "solver/heuristic/SimulatedAnnealingSolver.h"
#include "mutation/SwapMutation.h"
#include "mutation/ReverseMutation.h"
#include <iostream>
#include <memory>
#include <vector>

int main()
{
    try
    {
        // Initialize Gurobi environment
        GRBEnv env = GRBEnv();

        // Read TSP data
        TSPLibReader reader("/workspace/data/fl1400.tsp");
        auto cities = reader.readData();

        // Solve TSP using Gurobi
        TSPSolver solver(env, cities);
        solver.solve();

        // Solve TSP using Simulated Annealing with multiple mutation strategies
        std::vector<std::shared_ptr<MutationStrategy>> mutations = {
            std::make_shared<SwapMutation>(),
            std::make_shared<ReverseMutation>()};

        SimulatedAnnealingSolver saSolver(cities, 1000.0, 0.995, 10000, mutations);
        saSolver.solve();
        std::cout << "Simulated Annealing (Multiple Mutations) Best Distance: " << saSolver.getBestDistance() << std::endl;
    }
    catch (GRBException &e)
    {
        std::cout << "Gurobi error: " << e.getMessage() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Unknown error." << std::endl;
    }
}
