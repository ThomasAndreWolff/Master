#ifndef GENETIC_ALGORITHM_PARADISEO_H
#define GENETIC_ALGORITHM_PARADISEO_H

#include <utility>
#include <vector>

class GeneticAlgorithmParadiseo {
  public:
    GeneticAlgorithmParadiseo(const std::vector<std::pair<double, double>>& cities);
    void             solve();
    double           getBestDistance() const;
    std::vector<int> getBestTour() const;

  private:
    const std::vector<std::pair<double, double>>& cities;
    std::vector<int>                              bestTour;
    double                                        bestDistance;
};

#endif // GENETIC_ALGORITHM_PARADISEO_H