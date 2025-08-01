#include "mdvsp/MDVSPConfigurator.hpp"
#include <iostream>

MDVSPConfigurator::MDVSPConfigurator() {}

MDVSPConfigurator::~MDVSPConfigurator() {}

void MDVSPConfigurator::configureSolver(GRBModel* model) {
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

void MDVSPConfigurator::setTimeLimit(GRBModel* model, double timeLimit) {
    if (timeLimit > 0.0) {
        model->set(GRB_DoubleParam_TimeLimit, timeLimit);
        std::cout << "Time limit set to " << timeLimit << " seconds" << std::endl;
    }
}

void MDVSPConfigurator::setMIPStrategy(GRBModel* model) {
    // MIP strategy - emphasize finding good feasible solutions quickly
    model->set(GRB_IntParam_MIPFocus, 1); // Focus on feasibility
}

void MDVSPConfigurator::setPresolveSettings(GRBModel* model) {
    // Presolve settings - aggressive for large problems
    model->set(GRB_IntParam_Presolve, 2);   // Aggressive presolve
    model->set(GRB_IntParam_PrePasses, 10); // More presolve passes
}

void MDVSPConfigurator::setCuttingPlanes(GRBModel* model) {
    // Cuts - enable all relevant cuts for vehicle scheduling
    model->set(GRB_IntParam_Cuts, 2); // Aggressive cuts
    model->set(GRB_IntParam_CliqueCuts, 2);
    model->set(GRB_IntParam_CoverCuts, 2);
    model->set(GRB_IntParam_FlowCoverCuts, 2);
    model->set(GRB_IntParam_FlowPathCuts, 2);
    model->set(GRB_IntParam_GomoryPasses, 10);
}

void MDVSPConfigurator::setHeuristics(GRBModel* model) {
    // Heuristics - important for finding good solutions in vehicle scheduling
    model->set(GRB_DoubleParam_Heuristics, 0.1); // 10% time on heuristics
    model->set(GRB_IntParam_PumpPasses, 10);
    model->set(GRB_IntParam_RINS, 25); // RINS heuristic frequency
}

void MDVSPConfigurator::setBranchingStrategy(GRBModel* model) {
    // Branching strategy
    model->set(GRB_IntParam_VarBranch, 3); // Strong branching

    // Node selection
    model->set(GRB_IntParam_NodeMethod, 1); // Dual simplex for nodes
}

void MDVSPConfigurator::setMemoryAndThreading(GRBModel* model) {
    // Memory and threading
    model->set(GRB_IntParam_Threads, 0);            // Use all available threads
    model->set(GRB_DoubleParam_NodefileStart, 2.0); // Start using disk at 2GB
}

void MDVSPConfigurator::setTolerances(GRBModel* model) {
    // Tolerances - slightly relaxed for large problems
    model->set(GRB_DoubleParam_MIPGap, 0.01);   // 1% optimality gap
    model->set(GRB_DoubleParam_MIPGapAbs, 1.0); // Absolute gap of 1
}

void MDVSPConfigurator::setOutputControl(GRBModel* model) {
    // Output control
    model->set(GRB_IntParam_LogToConsole, 1);
    model->set(GRB_IntParam_DisplayInterval, 10); // Log every 10 seconds
}
