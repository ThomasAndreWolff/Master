#pragma once

#include "gurobi_c++.h"

/**
 * Analyzes MDVSP problem structure and provides statistics
 */
class MDVSPAnalyzer {
  public:
    MDVSPAnalyzer();
    ~MDVSPAnalyzer();

    /**
     * Analyze the problem structure
     * @param model The Gurobi model to analyze
     */
    void analyzeProblemStructure(GRBModel* model);

    /**
     * Print detailed problem statistics
     * @param model The Gurobi model to analyze
     */
    void printProblemStatistics(GRBModel* model);

    /**
     * Extract and print solution details
     * @param model The Gurobi model with solution
     * @return Number of active variables
     */
    int extractSolutionDetails(GRBModel* model);

  private:
    /**
     * Count variables by type
     */
    void countVariableTypes(GRBVar* vars,
                            int     numVars,
                            int&    binaryVars,
                            int&    integerVars,
                            int&    continuousVars);

    /**
     * Analyze objective coefficients
     */
    void analyzeObjectiveCoefficients(GRBVar* vars,
                                      int     numVars,
                                      double& minObj,
                                      double& maxObj,
                                      int&    objVars);
};
