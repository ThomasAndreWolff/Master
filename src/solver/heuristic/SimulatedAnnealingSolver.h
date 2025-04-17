#ifndef SIMULATED_ANNEALING_SOLVER_H
#define SIMULATED_ANNEALING_SOLVER_H

#include <chrono>
#include <iostream> // Add for error reporting
#include <limits>
#include <memory>
#include <random>
#include <utility>
#include <vector>
#include "mutation/MutationStrategy.h" // Include the refactored MutationStrategy header

class SimulatedAnnealingSolver {
  private:
    const std::vector<std::pair<double, double>>& cities;
    std::vector<int>                              bestTour;
    double                                        bestDistance;
    std::mt19937                                  rng;

    double initialTemp;
    double coolingRate;
    int    maxIterations;

    std::vector<std::shared_ptr<MutationStrategy>> mutationStrategies;

    double calculateDistance(const std::pair<double, double>& a, const std::pair<double, double>& b);
    double calculateTourDistance(const std::vector<int>& tour);

  public:
    SimulatedAnnealingSolver(const std::vector<std::pair<double, double>>&         cities,
                             double                                                initialTemp,
                             double                                                coolingRate,
                             int                                                   maxIterations,
                             const std::vector<std::shared_ptr<MutationStrategy>>& mutationStrategies);
    void                    solve();
    const std::vector<int>& getBestTour() const;
    double                  getBestDistance() const;
};

#endif // SIMULATED_ANNEALING_SOLVER_H
