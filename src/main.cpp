#include "gurobi_c++.h"
#include <iostream>

int main() {
    try {
        GRBEnv env = GRBEnv();
        GRBModel model = GRBModel(env);


        // Variables
        GRBVar x = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "x");
        GRBVar y = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "y");

        // Objective: maximize x + y
        model.setObjective(x + y, GRB_MINIMIZE);

        // Constraints: x + 2y <= 4
        model.addConstr(x + 2 * y <= 4, "c0");

        model.optimize();

        std::cout << "x: " << x.get(GRB_DoubleAttr_X) << std::endl;
        std::cout << "y: " << y.get(GRB_DoubleAttr_X) << std::endl;

    } catch (GRBException &e) {
        std::cout << "Gurobi error: " << e.getMessage() << std::endl;
    } catch (...) {
        std::cout << "Unknown error." << std::endl;
    }
}
