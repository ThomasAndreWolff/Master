#include "reader/TSPLibReader.h"
#include "solver/TSPSolver.h"
#include <iostream>

int main()
{
    try
    {
        // Initialize Gurobi environment
        GRBEnv env = GRBEnv();

        // Read TSP data
        TSPLibReader reader("/workspace/data/d15112.tsp");
        auto cities = reader.readData();

        // Solve TSP
        TSPSolver solver(env, cities);
        solver.solve();
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
