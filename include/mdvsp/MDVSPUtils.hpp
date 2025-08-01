#pragma once

#include <string>

/**
 * Utility functions for MDVSP solver
 */
namespace MDVSPUtils {
    
    /**
     * Convert Gurobi status to human-readable string
     */
    std::string statusToString(int status);
    
    /**
     * Print formatted header
     */
    void printHeader(const std::string& title);
    
    /**
     * Print progress indicator
     */
    void printProgress(const std::string& message);
    
    /**
     * Print warning message
     */
    void printWarning(const std::string& message);
    
    /**
     * Print error message
     */
    void printError(const std::string& message);
}
