#pragma once

#include <string>
#include <vector>

namespace exact {

/**
 * Represents a single MDVSP solution
 */
class MDVSPSolution {
  public:
    /**
     * Constructor
     */
    MDVSPSolution(int                solutionNumber = 0,
                  double             objectiveValue = 0.0,
                  double             solutionTime   = 0.0,
                  double             gap            = 0.0,
                  const std::string& status         = "UNKNOWN",
                  const std::string& problemName    = "",
                  int                numVariables   = 0,
                  int                numConstraints = 0);

    /**
     * Destructor
     */
    ~MDVSPSolution();

    // Public members for easy access
    int         solution_number;
    double      objective_value;
    double      solution_time_seconds;
    double      gap;
    std::string status;
    std::string problem_name;
    int         num_variables;
    int         num_constraints;

    // Metadata
    std::string timestamp;
    std::string solver_run_id;

    // Variable values (if available)
    std::vector<double>      variable_values;
    std::vector<std::string> variable_names;

    /**
     * Check if solution has variable data
     */
    bool hasVariableData() const;

    /**
     * Print solution summary
     */
    void printSummary() const;
};

} // namespace exact
