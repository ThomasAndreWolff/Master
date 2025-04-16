#include "reader/TSPLibReader.h"
#include "solver/exact/TSPSolver.h"
#include "solver/heuristic/SimulatedAnnealingSolver.h"
#include "mutation/SwapMutation.h"
#include "mutation/ReverseMutation.h"
#include <iostream>
#include <memory>

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

        // Solve TSP using Simulated Annealing with swap mutation
        auto swapMutation = std::make_shared<SwapMutation>();
        SimulatedAnnealingSolver saSolverSwap(cities, 1000.0, 0.995, 10000, swapMutation);
        saSolverSwap.solve();
        std::cout << "Simulated Annealing (Swap Mutation) Best Distance: " << saSolverSwap.getBestDistance() << std::endl;

        // Solve TSP using Simulated Annealing with reverse mutation
        auto reverseMutation = std::make_shared<ReverseMutation>();
        SimulatedAnnealingSolver saSolverReverse(cities, 1000.0, 0.995, 10000, reverseMutation);
        saSolverReverse.solve();
        std::cout << "Simulated Annealing (Reverse Mutation) Best Distance: " << saSolverReverse.getBestDistance() << std::endl;
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
