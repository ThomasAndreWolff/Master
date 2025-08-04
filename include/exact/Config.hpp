#pragma once

#include "gurobi_c++.h"

namespace exact {

/**
 * Configures Gurobi solver parameters for optimization problems
 */
class SolverConfigurator {
  public:
    SolverConfigurator();
    ~SolverConfigurator();

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

    /**
     * Find model file for given problem
     * @param problemName Name of the problem instance
     * @param modelDirectory Directory to search in
     * @param preferredFormat Preferred file format (.lp or .mps)
     * @return Path to model file or empty string if not found
     */
    static std::string findModelFile(const std::string& problemName,
                                     const std::string& modelDirectory,
                                     const std::string& preferredFormat);

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

} // namespace exact
