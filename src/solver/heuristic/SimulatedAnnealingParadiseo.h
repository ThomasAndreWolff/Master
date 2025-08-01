#ifndef SIMULATED_ANNEALING_PARADISEO_H
#define SIMULATED_ANNEALING_PARADISEO_H

#include <utility>
#include <vector>

class SimulatedAnnealingParadiseo {
  public:
    SimulatedAnnealingParadiseo(const std::vector<std::pair<double, double>>& cities);
    void             solve();
    double           getBestDistance() const;
    std::vector<int> getBestTour() const;

  private:
    const std::vector<std::pair<double, double>>& cities;
    std::vector<int>                              bestTour;
    double                                        bestDistance;
};

#endif // SIMULATED_ANNEALING_PARADISEO_H