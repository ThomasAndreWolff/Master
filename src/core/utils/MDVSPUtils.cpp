#include "mdvsp/MDVSPUtils.hpp"
#include <iostream>

namespace MDVSPUtils {

std::string statusToString(int status) {
    switch (status) {
        case 1:
            return "LOADED";
        case 2:
            return "OPTIMAL";
        case 3:
            return "INFEASIBLE";
        case 4:
            return "INF_OR_UNBD";
        case 5:
            return "UNBOUNDED";
        case 6:
            return "CUTOFF";
        case 7:
            return "ITERATION_LIMIT";
        case 8:
            return "NODE_LIMIT";
        case 9:
            return "TIME_LIMIT";
        case 10:
            return "SOLUTION_LIMIT";
        case 11:
            return "INTERRUPTED";
        case 12:
            return "NUMERIC";
        case 13:
            return "SUBOPTIMAL";
        case 14:
            return "INPROGRESS";
        case 15:
            return "USER_OBJ_LIMIT";
        default:
            return "UNKNOWN";
    }
}

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void printProgress(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

void printWarning(const std::string& message) {
    std::cout << "[WARNING] " << message << std::endl;
}

void printError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}
} // namespace MDVSPUtils
