#include "mdvsp/MDVSPFileHandler.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

MDVSPFileHandler::MDVSPFileHandler() {}

MDVSPFileHandler::~MDVSPFileHandler() {}

bool MDVSPFileHandler::loadMPSFile(GRBModel* model, const std::string& filename) {
    try {
        if (!validateFile(filename)) {
            return false;
        }

        auto start = std::chrono::high_resolution_clock::now();
        model->read(filename);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

        std::cout << "MPS file loaded in " << duration.count() << " seconds" << std::endl;
        return true;
    }
    catch (GRBException& e) {
        std::cerr << "Error loading MPS file: " << e.getMessage() << std::endl;
        return false;
    }
}

bool MDVSPFileHandler::loadLPFile(GRBModel* model, const std::string& filename) {
    try {
        if (!validateFile(filename)) {
            return false;
        }

        auto start = std::chrono::high_resolution_clock::now();
        model->read(filename);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

        std::cout << "LP file loaded in " << duration.count() << " seconds" << std::endl;
        return true;
    }
    catch (GRBException& e) {
        std::cerr << "Error loading LP file: " << e.getMessage() << std::endl;
        return false;
    }
}

bool MDVSPFileHandler::loadProblemFile(GRBModel* model, const std::string& filename) {
    // Auto-detect format based on extension
    std::string extension;
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filename.substr(dotPos + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    }

    if (extension == "mps") {
        return loadMPSFile(model, filename);
    }
    else if (extension == "lp") {
        return loadLPFile(model, filename);
    }
    else {
        std::cerr << "Unsupported file format. Use .mps or .lp files." << std::endl;
        return false;
    }
}

bool MDVSPFileHandler::exportSolution(GRBModel*          model,
                                      const std::string& filename,
                                      double             objectiveValue,
                                      double             solutionTime,
                                      bool               isOptimal) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Cannot open file for writing: " << filename << std::endl;
            return false;
        }

        file << "MDVSP Solution Export" << std::endl;
        file << "===================" << std::endl;
        file << "Objective Value: " << objectiveValue << std::endl;
        file << "Solution Time: " << solutionTime << " seconds" << std::endl;
        file << "Status: " << (isOptimal ? "OPTIMAL" : "FEASIBLE") << std::endl;
        file << std::endl;

        // Write solution variables
        file << "Solution Variables:" << std::endl;
        GRBVar* vars    = model->getVars();
        int     numVars = model->get(GRB_IntAttr_NumVars);

        for (int i = 0; i < numVars; i++) {
            double value = vars[i].get(GRB_DoubleAttr_X);
            if (value > 1e-6) { // Only write non-zero values
                file << vars[i].get(GRB_StringAttr_VarName) << " = " << value << std::endl;
            }
        }

        delete[] vars;
        file.close();

        std::cout << "Solution exported to: " << filename << std::endl;
        return true;
    }
    catch (GRBException& e) {
        std::cerr << "Error exporting solution: " << e.getMessage() << std::endl;
        return false;
    }
    catch (std::exception& e) {
        std::cerr << "Error exporting solution: " << e.what() << std::endl;
        return false;
    }
}

bool MDVSPFileHandler::validateFile(const std::string& filename) {
    std::ifstream fileCheck(filename);
    if (!fileCheck.good()) {
        std::cerr << "Error: Cannot access file " << filename << std::endl;
        return false;
    }
    fileCheck.close();
    return true;
}
