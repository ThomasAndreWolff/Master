#pragma once

#include <string>
#include "gurobi_c++.h"

/**
 * Handles file I/O operations for MDVSP problems
 */
class MDVSPFileHandler {
  public:
    MDVSPFileHandler();
    ~MDVSPFileHandler();

    /**
     * Load MPS file into a Gurobi model
     * @param model The Gurobi model to load into
     * @param filename Path to the MPS file
     * @return true if successful, false otherwise
     */
    bool loadMPSFile(GRBModel* model, const std::string& filename);

    /**
     * Load LP file into a Gurobi model
     * @param model The Gurobi model to load into
     * @param filename Path to the LP file
     * @return true if successful, false otherwise
     */
    bool loadLPFile(GRBModel* model, const std::string& filename);

    /**
     * Load optimization problem file into a Gurobi model (auto-detects format)
     * Supports both MPS and LP formats
     * @param model The Gurobi model to load into
     * @param filename Path to the problem file
     * @return true if successful, false otherwise
     */
    bool loadProblemFile(GRBModel* model, const std::string& filename);

    /**
     * Export solution to file
     * @param model The Gurobi model with solution
     * @param filename Output file path
     * @param objectiveValue The objective value
     * @param solutionTime Time taken to solve
     * @param isOptimal Whether solution is optimal
     * @return true if successful, false otherwise
     */
    bool exportSolution(GRBModel*          model,
                        const std::string& filename,
                        double             objectiveValue,
                        double             solutionTime,
                        bool               isOptimal);

  private:
    /**
     * Perform basic file validation
     */
    bool validateFile(const std::string& filename);
};
