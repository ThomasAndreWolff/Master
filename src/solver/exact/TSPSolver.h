#ifndef TSP_SOLVER_H
#define TSP_SOLVER_H

#include <utility>
#include <vector>
#include "gurobi_c++.h"

class TSPSolver {
  private:
    GRBEnv&                                       env;
    const std::vector<std::pair<double, double>>& cities;
    std::vector<int>                              heuristicTour; // Store the heuristic solution

    double calculateDistance(const std::pair<double, double>& a, const std::pair<double, double>& b);

    class TSPCallback : public GRBCallback {
      private:
        const std::vector<int>&                       heuristicTour;
        const std::vector<std::pair<double, double>>& cities;
        GRBModel&                                     model; // Reference to the model

      public:
        TSPCallback(const std::vector<int>&                       heuristicTour,
                    const std::vector<std::pair<double, double>>& cities,
                    GRBModel&                                     model)
          : heuristicTour(heuristicTour), cities(cities), model(model) {}

      protected:
        void callback() override;
    };

  public:
    TSPSolver(GRBEnv& env, const std::vector<std::pair<double, double>>& cities);
    void setHeuristicSolution(const std::vector<int>& tour); // Set heuristic solution
    void solve();
};

#endif // TSP_SOLVER_H
