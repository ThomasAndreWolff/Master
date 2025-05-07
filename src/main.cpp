#include <iostream>
#include <memory>
#include <vector>
#include "mutation/ReverseMutation.h"
#include "mutation/SwapMutation.h"
#include "reader/TSPLibReader.h"
#include "solver/exact/TSPSolver.h"
#include "solver/heuristic/SimulatedAnnealingSolver.h"

int main() {
    try {
        GRBEnv env = GRBEnv();

        TSPLibReader reader("/workspace/data/pr1002.tsp");
        auto         cities = reader.readData();

        std::vector<std::shared_ptr<MutationStrategy>> mutations = {
            std::make_shared<SwapMutation>(), std::make_shared<ReverseMutation>()};

        SimulatedAnnealingSolver saSolver(cities, 1000.0, 0.995, 10000, mutations);
        saSolver.solve();
        std::cout << "Simulated Annealing (Multiple Mutations) Best Distance: "
                  << saSolver.getBestDistance() << std::endl;

        TSPSolver solver(env, cities);
        solver.setHeuristicSolution(saSolver.getBestTour());
        solver.solve();
    }
    catch (GRBException& e) {
        std::cout << "Gurobi error: " << e.getMessage() << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cout << "Unknown error." << std::endl;
    }
}
