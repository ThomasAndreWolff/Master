#include "exact/Callback.hpp"
#include <iostream>

namespace exact {

SolverCallback::SolverCallback(bool verbose)
  : verbose_(verbose), solutionCount_(0), bestObjective_(GRB_INFINITY) {}

SolverCallback::~SolverCallback() = default;

void SolverCallback::callback() {
    try {
        if (where == GRB_CB_MIPSOL) {
            handleMIPSOL();
        }
        else if (where == GRB_CB_MIPNODE) {
            handleMIPNODE();
        }
    }
    catch (GRBException& e) {
        std::cerr << "Callback error: " << e.getMessage() << std::endl;
    }
}

void SolverCallback::handleMIPSOL() {
    solutionCount_++;
    double objVal = getDoubleInfo(GRB_CB_MIPSOL_OBJ);

    if (objVal < bestObjective_) {
        bestObjective_ = objVal;

        if (verbose_) {
            logMessage("New best solution found: " + std::to_string(objVal));
        }
    }
}

void SolverCallback::handleMIPNODE() {
    if (verbose_) {
        double objBound = getDoubleInfo(GRB_CB_MIPNODE_OBJBND);
        double objBest  = getDoubleInfo(GRB_CB_MIPNODE_OBJBST);

        if (objBest < GRB_INFINITY) {
            double gap = (objBest - objBound) / objBest;
            logMessage("Node processed. Gap: " + std::to_string(gap * 100) + "%");
        }
    }
}

void SolverCallback::logMessage(const std::string& message) const {
    if (verbose_) {
        std::cout << "[Callback] " << message << std::endl;
    }
}

} // namespace exact
