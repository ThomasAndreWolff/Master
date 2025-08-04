#include "core/Solver.hpp"
#include <iostream>
#include <iomanip>
#include <string>

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -d, --data <path>        Data directory path (default: /workspace/data/short)" << std::endl;
    std::cout << "  -m, --method <method>    Solution method: greedy, exact, or both (default: both)" << std::endl;
    std::cout << "  -t, --time <seconds>     Time limit in seconds (default: 300)" << std::endl;
    std::cout << "  -g, --gap <gap>          Optimality gap for exact solver (default: 0.01)" << std::endl;
    std::cout << "  -v, --verbose            Enable verbose output" << std::endl;
    std::cout << "  -h, --help               Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Methods:" << std::endl;
    std::cout << "  greedy                   Fast heuristic algorithm" << std::endl;
    std::cout << "  exact                    Optimal solution using Gurobi" << std::endl;
    std::cout << "  both                     Run greedy first, then exact with warm start" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== Unified MDVSP Solver ===" << std::endl;
    std::cout << "Multi-Depot Vehicle Scheduling Problem Solver" << std::endl;
    std::cout << "Supports: Greedy Heuristic, Exact Optimization, and Hybrid Approach" << std::endl;
    std::cout << std::endl;

    // Default parameters
    std::string dataPath = "/workspace/data/short";
    std::string method = "both";
    double timeLimit = 300.0;
    double optimalityGap = 0.01;
    bool verbose = false;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-d" || arg == "--data") {
            if (i + 1 < argc) {
                dataPath = argv[++i];
            } else {
                std::cerr << "Error: --data requires a path argument" << std::endl;
                return 1;
            }
        } else if (arg == "-m" || arg == "--method") {
            if (i + 1 < argc) {
                method = argv[++i];
            } else {
                std::cerr << "Error: --method requires an argument" << std::endl;
                return 1;
            }
        } else if (arg == "-t" || arg == "--time") {
            if (i + 1 < argc) {
                timeLimit = std::stod(argv[++i]);
            } else {
                std::cerr << "Error: --time requires a number" << std::endl;
                return 1;
            }
        } else if (arg == "-g" || arg == "--gap") {
            if (i + 1 < argc) {
                optimalityGap = std::stod(argv[++i]);
            } else {
                std::cerr << "Error: --gap requires a number" << std::endl;
                return 1;
            }
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else {
            // Positional arguments for backward compatibility
            if (i == 1) dataPath = arg;
            else if (i == 2) method = arg;
            else if (i == 3) timeLimit = std::stod(arg);
        }
    }

    // Extract problem name from data path
    std::string problemName = "unknown";
    size_t lastSlash = dataPath.find_last_of('/');
    if (lastSlash != std::string::npos) {
        problemName = dataPath.substr(lastSlash + 1);
    } else {
        problemName = dataPath;
    }

    std::cout << "Configuration:" << std::endl;
    std::cout << "  Data path: " << dataPath << std::endl;
    std::cout << "  Problem: " << problemName << std::endl;
    std::cout << "  Method: " << method << std::endl;
    std::cout << "  Time limit: " << timeLimit << "s" << std::endl;
    if (method == "exact" || method == "both") {
        std::cout << "  Optimality gap: " << (optimalityGap * 100) << "%" << std::endl;
    }
    std::cout << "  Verbose: " << (verbose ? "enabled" : "disabled") << std::endl;
    std::cout << std::endl;

    try {
        // Initialize solver
        MDVSPSolver solver;

        // Load problem
        std::cout << "Loading problem data..." << std::endl;
        if (!solver.loadProblem(dataPath)) {
            std::cerr << "ERROR: Failed to load problem from: " << dataPath << std::endl;
            return 1;
        }
        std::cout << "✓ Problem data loaded successfully" << std::endl;
        std::cout << std::endl;

        core::UnifiedSolution greedySolution;
        core::UnifiedSolution exactSolution;

        // Execute based on method
        if (method == "greedy" || method == "both") {
            std::cout << "=== Phase 1: Greedy Solution ===" << std::endl;
            
            MDVSPSolver::SolverConfig greedyConfig;
            greedyConfig.method = "greedy";
            greedyConfig.problemName = problemName;
            greedyConfig.verbose = verbose;
            greedyConfig.timeLimit = std::min(timeLimit, 60.0); // Greedy is fast
            
            greedySolution = solver.solve(greedyConfig);
            
            std::cout << "Greedy Results:" << std::endl;
            std::cout << "  Status: " << greedySolution.status << std::endl;
            std::cout << "  Objective: " << std::fixed << std::setprecision(2) 
                      << greedySolution.objectiveValue << std::endl;
            std::cout << "  Time: " << std::fixed << std::setprecision(3) 
                      << greedySolution.solutionTimeSeconds << "s" << std::endl;
            std::cout << "  Vehicles: " << greedySolution.getNumVehicles() << std::endl;
            std::cout << "  Journeys: " << greedySolution.getNumJourneys() << std::endl;
            std::cout << std::endl;
        }

        if (method == "exact" || method == "both") {
            std::cout << "=== Phase 2: Exact Solution ===" << std::endl;
            
            if (method == "both" && greedySolution.status == "FEASIBLE") {
                std::cout << "Using greedy solution as warm start..." << std::endl;
                exactSolution = solver.solveWithWarmStart(greedySolution, "exact");
            } else {
                MDVSPSolver::SolverConfig exactConfig;
                exactConfig.method = "exact";
                exactConfig.problemName = problemName;
                exactConfig.verbose = verbose;
                exactConfig.timeLimit = timeLimit;
                exactConfig.optimalityGap = optimalityGap;
                
                exactSolution = solver.solve(exactConfig);
            }
            
            std::cout << "Exact Results:" << std::endl;
            std::cout << "  Status: " << exactSolution.status << std::endl;
            if (exactSolution.status == "OPTIMAL" || exactSolution.status == "TIME_LIMIT") {
                std::cout << "  Objective: " << std::fixed << std::setprecision(2) 
                          << exactSolution.objectiveValue << std::endl;
            }
            std::cout << "  Time: " << std::fixed << std::setprecision(3) 
                      << exactSolution.solutionTimeSeconds << "s" << std::endl;
            std::cout << std::endl;
        }

        // Final summary
        std::cout << "=== Final Summary ===" << std::endl;
        
        if (method == "both") {
            std::cout << "Greedy vs Exact comparison:" << std::endl;
            
            if (greedySolution.status == "FEASIBLE") {
                std::cout << "  Greedy: " << std::fixed << std::setprecision(2) 
                          << greedySolution.objectiveValue << " in " 
                          << std::setprecision(3) << greedySolution.solutionTimeSeconds << "s" << std::endl;
            }
            
            if (exactSolution.status == "OPTIMAL" || exactSolution.status == "TIME_LIMIT") {
                std::cout << "  Exact:  " << std::fixed << std::setprecision(2) 
                          << exactSolution.objectiveValue << " in " 
                          << std::setprecision(3) << exactSolution.solutionTimeSeconds << "s" << std::endl;
                
                if (greedySolution.status == "FEASIBLE" && exactSolution.objectiveValue > 0) {
                    double improvement = ((greedySolution.objectiveValue - exactSolution.objectiveValue) / 
                                        greedySolution.objectiveValue) * 100.0;
                    std::cout << "  Improvement: " << std::fixed << std::setprecision(1) 
                              << improvement << "%" << std::endl;
                }
            }
        }

        // Determine best solution and return code
        core::UnifiedSolution* bestSolution = nullptr;
        
        if (method == "greedy") {
            bestSolution = &greedySolution;
        } else if (method == "exact") {
            bestSolution = &exactSolution;
        } else { // both
            if (exactSolution.status == "OPTIMAL" || exactSolution.status == "TIME_LIMIT") {
                bestSolution = &exactSolution;
                std::cout << "✓ Best solution: Exact solver" << std::endl;
            } else if (greedySolution.status == "FEASIBLE") {
                bestSolution = &greedySolution;
                std::cout << "✓ Best solution: Greedy solver (exact failed)" << std::endl;
            }
        }

        if (bestSolution && (bestSolution->status == "FEASIBLE" || bestSolution->status == "OPTIMAL" || bestSolution->status == "TIME_LIMIT")) {
            std::cout << "✓ Solution completed successfully" << std::endl;
            return 0;
        } else {
            std::cout << "✗ No feasible solution found" << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "\nERROR: " << e.what() << std::endl;
        return 2;
    }
}
