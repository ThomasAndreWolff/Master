#pragma once

#include <string>
#include "gurobi_c++.h"

namespace exact {

/**
 * Callback handler for exact solver optimization
 */
class SolverCallback : public GRBCallback {
  public:
    /**
     * Constructor
     */
    SolverCallback(bool verbose = false);

    /**
     * Destructor
     */
    ~SolverCallback();

    /**
     * Main callback method called by Gurobi
     */
    void callback() override;

    /**
     * Get solution count
     */
    int getSolutionCount() const { return solutionCount_; }

    /**
     * Get best objective found
     */
    double getBestObjective() const { return bestObjective_; }

  private:
    bool   verbose_;
    int    solutionCount_;
    double bestObjective_;

    void handleMIPSOL();
    void handleMIPNODE();
    void logMessage(const std::string& message) const;
};

} // namespace exact
