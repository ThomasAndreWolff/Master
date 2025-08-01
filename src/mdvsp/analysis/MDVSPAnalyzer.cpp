#include "mdvsp/MDVSPAnalyzer.hpp"
#include <algorithm>
#include <iostream>

MDVSPAnalyzer::MDVSPAnalyzer() {}

MDVSPAnalyzer::~MDVSPAnalyzer() {}

void MDVSPAnalyzer::analyzeProblemStructure(GRBModel* model) {
    try {
        // Get basic problem statistics
        int numVars    = model->get(GRB_IntAttr_NumVars);
        int numConstrs = model->get(GRB_IntAttr_NumConstrs);
        int numNZs     = model->get(GRB_IntAttr_NumNZs);

        std::cout << "\nProblem Structure Analysis:" << std::endl;
        std::cout << "- Variables: " << numVars << std::endl;
        std::cout << "- Constraints: " << numConstrs << std::endl;
        std::cout << "- Non-zeros: " << numNZs << std::endl;

        // Analyze variable types for MDVSP structure
        GRBVar* vars       = model->getVars();
        int     binaryVars = 0, integerVars = 0, continuousVars = 0;

        countVariableTypes(vars, numVars, binaryVars, integerVars, continuousVars);

        std::cout << "Variable Types:" << std::endl;
        std::cout << "- Binary: " << binaryVars << std::endl;
        std::cout << "- Integer: " << integerVars << std::endl;
        std::cout << "- Continuous: " << continuousVars << std::endl;

        delete[] vars;
    }
    catch (GRBException& e) {
        std::cerr << "Error analyzing problem structure: " << e.getMessage() << std::endl;
    }
}

void MDVSPAnalyzer::printProblemStatistics(GRBModel* model) {
    try {
        std::cout << "\nMDVSP Problem Statistics:" << std::endl;
        std::cout << "========================================" << std::endl;

        // Model sense (minimize/maximize)
        int modelSense = model->get(GRB_IntAttr_ModelSense);
        std::cout << "Objective: " << (modelSense == GRB_MINIMIZE ? "Minimize" : "Maximize")
                  << std::endl;

        // Get objective coefficients range
        GRBVar* vars    = model->getVars();
        int     numVars = model->get(GRB_IntAttr_NumVars);

        double minObj = GRB_INFINITY, maxObj = -GRB_INFINITY;
        int    objVars = 0;

        analyzeObjectiveCoefficients(vars, numVars, minObj, maxObj, objVars);

        std::cout << "Objective coefficients:" << std::endl;
        std::cout << "- Variables with obj coeffs: " << objVars << std::endl;
        if (objVars > 0) {
            std::cout << "- Min coefficient: " << minObj << std::endl;
            std::cout << "- Max coefficient: " << maxObj << std::endl;
        }

        delete[] vars;
    }
    catch (GRBException& e) {
        std::cerr << "Error printing problem statistics: " << e.getMessage() << std::endl;
    }
}

int MDVSPAnalyzer::extractSolutionDetails(GRBModel* model) {
    try {
        std::cout << "\nExtracting solution details..." << std::endl;

        GRBVar* vars    = model->getVars();
        int     numVars = model->get(GRB_IntAttr_NumVars);

        int    activeVars = 0;
        double totalCost  = 0.0;

        for (int i = 0; i < numVars; i++) {
            double value = vars[i].get(GRB_DoubleAttr_X);
            if (value > 0.5) { // For binary variables
                activeVars++;
                double objCoeff = vars[i].get(GRB_DoubleAttr_Obj);
                totalCost += value * objCoeff;
            }
        }

        std::cout << "- Active variables: " << activeVars << std::endl;
        std::cout << "- Total calculated cost: " << totalCost << std::endl;

        delete[] vars;
        return activeVars;
    }
    catch (GRBException& e) {
        std::cerr << "Error extracting solution details: " << e.getMessage() << std::endl;
        return 0;
    }
}

void MDVSPAnalyzer::countVariableTypes(GRBVar* vars,
                                       int     numVars,
                                       int&    binaryVars,
                                       int&    integerVars,
                                       int&    continuousVars) {
    for (int i = 0; i < numVars; i++) {
        char vtype = vars[i].get(GRB_CharAttr_VType);
        switch (vtype) {
            case GRB_BINARY:
                binaryVars++;
                break;
            case GRB_INTEGER:
                integerVars++;
                break;
            case GRB_CONTINUOUS:
                continuousVars++;
                break;
        }
    }
}

void MDVSPAnalyzer::analyzeObjectiveCoefficients(GRBVar* vars,
                                                 int     numVars,
                                                 double& minObj,
                                                 double& maxObj,
                                                 int&    objVars) {
    for (int i = 0; i < numVars; i++) {
        double objCoeff = vars[i].get(GRB_DoubleAttr_Obj);
        if (objCoeff != 0.0) {
            objVars++;
            minObj = std::min(minObj, objCoeff);
            maxObj = std::max(maxObj, objCoeff);
        }
    }
}
