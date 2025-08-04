#include "core/SolutionTransformer.hpp"
#include "core/UnifiedSolutionDatabase.hpp"
#include "data/DataLoader.hpp"
#include "greedy/GreedyMDVSPSolver.hpp"
#include "greedy/GreedySolutionPrinter.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "=== Greedy MDVSP Solver ===" << std::endl;
    std::cout << "A fast heuristic for the Multi-Depot Vehicle Scheduling Problem" << std::endl;
    std::cout << std::endl;

    // Default data directory
    std::string dataDir = "/workspace/data/short";

    // Parse command line arguments
    if (argc > 1) {
        dataDir = argv[1];
    }

    std::cout << "Loading data from: " << dataDir << std::endl;
    std::cout << std::endl;

    // Load data
    data::DataLoader dataLoader(dataDir);
    if (!dataLoader.loadAll()) {
        std::cerr << "Failed to load MDVSP data!" << std::endl;
        return 1;
    }

    // Create and configure solver
    greedy::GreedyMDVSPSolver solver(dataLoader);
    solver.setVerbose(true);

    // Solve the problem
    std::cout << std::endl;
    auto solution = solver.solve();
    std::cout << std::endl;

    // Print results
    greedy::GreedySolutionPrinter printer(dataLoader);
    printer.printSolution(solution, std::cout);

    std::cout << std::endl;

    // Validate solution
    printer.validateSolution(solution, std::cout);

    // Save solution to file
    std::string outputFile = "/workspace/results/greedy_solution.txt";
    if (printer.printToFile(solution, outputFile)) {
        std::cout << std::endl;
        std::cout << "Solution saved to: " << outputFile << std::endl;
    }
    else {
        std::cerr << "Failed to save solution to file!" << std::endl;
    }

    // Convert greedy solution to unified format and save to database
    std::cout << std::endl;
    std::cout << "Converting solution to unified format..." << std::endl;

    try {
        // Transform greedy solution to unified format
        core::UnifiedSolution unifiedSolution =
            core::SolutionTransformer::transform(solution, dataLoader, dataDir);

        std::cout << "Saving solution to database..." << std::endl;

        // Initialize and save to database
        core::UnifiedSolutionDatabase database("/workspace/database/mdvsp_solutions.db", true);
        if (database.initialize()) {
            int solutionId = database.saveSolution(unifiedSolution);
            if (solutionId > 0) {
                std::cout << "Solution successfully saved to database with ID: " << solutionId
                          << std::endl;
            }
            else {
                std::cerr << "Failed to save solution to database!" << std::endl;
            }
        }
        else {
            std::cerr << "Failed to initialize database!" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Database error: " << e.what() << std::endl;
    }

    return solution.isFeasible ? 0 : 1;
}
