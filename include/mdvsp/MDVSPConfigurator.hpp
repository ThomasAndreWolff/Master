#pragma once

#include "gurobi_c++.h"

/**
 * Configures Gurobi solver parameters for MDVSP problems
 */
class MDVSPConfigurator {
  public:
    MDVSPConfigurator();
    ~MDVSPConfigurator();

    /**
     * Configure Gurobi model for MDVSP solving
     * @param model The Gurobi model to configure
     */
    void configureSolver(GRBModel* model);

    /**
     * Set time limit for optimization
     * @param model The Gurobi model
     * @param timeLimit Time limit in seconds (0 = no limit)
     */
    void setTimeLimit(GRBModel* model, double timeLimit);

  private:
    /**
     * Set MIP strategy parameters
     */
    void setMIPStrategy(GRBModel* model);

    /**
     * Set presolve parameters
     */
    void setPresolveSettings(GRBModel* model);

    /**
     * Set cutting plane parameters
     */
    void setCuttingPlanes(GRBModel* model);

    /**
     * Set heuristic parameters
     */
    void setHeuristics(GRBModel* model);

    /**
     * Set branching and node selection
     */
    void setBranchingStrategy(GRBModel* model);

    /**
     * Set memory and threading parameters
     */
    void setMemoryAndThreading(GRBModel* model);

    /**
     * Set tolerance parameters
     */
    void setTolerances(GRBModel* model);

    /**
     * Set output control parameters
     */
    void setOutputControl(GRBModel* model);
};
