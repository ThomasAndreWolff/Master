#include "exact/Config.hpp"
#include <filesystem>
#include <iostream>

namespace exact {

SolverConfigurator::SolverConfigurator() {}

SolverConfigurator::~SolverConfigurator() {}

std::string SolverConfigurator::findModelFile(const std::string& problemName,
                                              const std::string& modelDirectory,
                                              const std::string& preferredFormat) {
    try {
        std::cout << "Searching for model file: " << problemName << std::endl;
        std::cout << "  Directory: " << modelDirectory << std::endl;
        std::cout << "  Preferred format: " << preferredFormat << std::endl;

        if (!std::filesystem::exists(modelDirectory)) {
            std::cerr << "Model directory does not exist: " << modelDirectory << std::endl;
            return "";
        }

        // First try with preferred format
        std::string preferredFile = modelDirectory + "/" + problemName + preferredFormat;
        if (std::filesystem::exists(preferredFile)) {
            std::cout << "Found model file: " << preferredFile << std::endl;
            return preferredFile;
        }

        // Try other formats
        std::vector<std::string> formats = {".lp", ".mps"};
        for (const auto& format : formats) {
            if (format != preferredFormat) {
                std::string file = modelDirectory + "/" + problemName + format;
                if (std::filesystem::exists(file)) {
                    std::cout << "Found model file: " << file << std::endl;
                    return file;
                }
            }
        }

        // Try to find any file containing the problem name
        for (const auto& entry : std::filesystem::directory_iterator(modelDirectory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.find(problemName) != std::string::npos &&
                    (filename.size() >= 3 && (filename.substr(filename.size() - 3) == ".lp" ||
                                              filename.substr(filename.size() - 4) == ".mps"))) {
                    std::cout << "Found matching model file: " << entry.path() << std::endl;
                    return entry.path().string();
                }
            }
        }

        std::cerr << "No model file found for problem: " << problemName << std::endl;
        return "";
    }
    catch (const std::exception& e) {
        std::cerr << "Error searching for model file: " << e.what() << std::endl;
        return "";
    }
}

void SolverConfigurator::configureSolver(GRBModel* model) {
    try {
        std::cout << "\nConfiguring Gurobi for MDVSP solving..." << std::endl;

        setMIPStrategy(model);
        setPresolveSettings(model);
        setCuttingPlanes(model);
        setHeuristics(model);
        setBranchingStrategy(model);
        setMemoryAndThreading(model);
        setTolerances(model);
        setOutputControl(model);

        std::cout << "Solver configuration complete." << std::endl;
    }
    catch (GRBException& e) {
        std::cerr << "Error configuring solver: " << e.getMessage() << std::endl;
    }
}

void SolverConfigurator::setTimeLimit(GRBModel* model, double timeLimit) {
    if (timeLimit > 0.0) {
        model->set(GRB_DoubleParam_TimeLimit, timeLimit);
        std::cout << "Time limit set to " << timeLimit << " seconds" << std::endl;
    }
}

void SolverConfigurator::setMIPStrategy(GRBModel* model) {
    // MIP strategy - emphasize finding good feasible solutions quickly
    model->set(GRB_IntParam_MIPFocus, 1); // Focus on feasibility
}

void SolverConfigurator::setPresolveSettings(GRBModel* model) {
    // Presolve settings - aggressive for large problems
    model->set(GRB_IntParam_Presolve, 2);   // Aggressive presolve
    model->set(GRB_IntParam_PrePasses, 10); // More presolve passes
}

void SolverConfigurator::setCuttingPlanes(GRBModel* model) {
    // Cuts - enable all relevant cuts for vehicle scheduling
    model->set(GRB_IntParam_Cuts, 2); // Aggressive cuts
    model->set(GRB_IntParam_CliqueCuts, 2);
    model->set(GRB_IntParam_CoverCuts, 2);
    model->set(GRB_IntParam_FlowCoverCuts, 2);
    model->set(GRB_IntParam_FlowPathCuts, 2);
    model->set(GRB_IntParam_GomoryPasses, 10);
}

void SolverConfigurator::setHeuristics(GRBModel* model) {
    // Heuristics - important for finding good solutions in vehicle scheduling
    model->set(GRB_DoubleParam_Heuristics, 0.1); // 10% time on heuristics
    model->set(GRB_IntParam_PumpPasses, 10);
    model->set(GRB_IntParam_RINS, 25); // RINS heuristic frequency
}

void SolverConfigurator::setBranchingStrategy(GRBModel* model) {
    // Branching strategy
    model->set(GRB_IntParam_VarBranch, 3); // Strong branching

    // Node selection
    model->set(GRB_IntParam_NodeMethod, 1); // Dual simplex for nodes
}

void SolverConfigurator::setMemoryAndThreading(GRBModel* model) {
    // Memory and threading
    model->set(GRB_IntParam_Threads, 0);            // Use all available threads
    model->set(GRB_DoubleParam_NodefileStart, 2.0); // Start using disk at 2GB
}

void SolverConfigurator::setTolerances(GRBModel* model) {
    // Tolerances - slightly relaxed for large problems
    model->set(GRB_DoubleParam_MIPGap, 0.01);   // 1% optimality gap
    model->set(GRB_DoubleParam_MIPGapAbs, 1.0); // Absolute gap of 1
}

void SolverConfigurator::setOutputControl(GRBModel* model) {
    // Output control
    model->set(GRB_IntParam_LogToConsole, 1);
    model->set(GRB_IntParam_DisplayInterval, 10); // Log every 10 seconds
}
} // namespace exact