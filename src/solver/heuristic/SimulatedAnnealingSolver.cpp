#include "solver/heuristic/SimulatedAnnealingSolver.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <random>

SimulatedAnnealingSolver::SimulatedAnnealingSolver(
    const std::vector<std::pair<double, double>>&         cities,
    double                                                initialTemp,
    double                                                coolingRate,
    int                                                   maxIterations,
    const std::vector<std::shared_ptr<MutationStrategy>>& mutationStrategies)
  : cities(cities),
    bestDistance(std::numeric_limits<double>::max()),
    initialTemp(initialTemp),
    coolingRate(coolingRate),
    maxIterations(maxIterations),
    mutationStrategies(mutationStrategies) {
    rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
}

double SimulatedAnnealingSolver::calculateDistance(const std::pair<double, double>& a,
                                                   const std::pair<double, double>& b) {
    return std::sqrt(std::pow(a.first - b.first, 2) + std::pow(a.second - b.second, 2));
}

double SimulatedAnnealingSolver::calculateTourDistance(const std::vector<int>& tour) {
    double distance = 0.0;

    // Validate tour size
    if (tour.size() != cities.size()) {
        std::cerr << "Warning: Tour size (" << tour.size() << ") doesn't match number of cities ("
                  << cities.size() << ")" << std::endl;
        return std::numeric_limits<double>::max();
    }

    // Check for duplicate cities
    std::vector<bool> visited(cities.size(), false);
    for (size_t i = 0; i < tour.size(); ++i) {
        if (tour[i] < 0 || tour[i] >= static_cast<int>(cities.size())) {
            std::cerr << "Warning: Invalid city index: " << tour[i] << std::endl;
            return std::numeric_limits<double>::max();
        }

        if (visited[tour[i]]) {
            std::cerr << "Warning: City " << tour[i] << " appears multiple times in tour"
                      << std::endl;
            return std::numeric_limits<double>::max();
        }
        visited[tour[i]] = true;
    }

    // Calculate distance correctly
    for (size_t i = 0; i < tour.size(); ++i) {
        int from = tour[i];
        int to   = tour[(i + 1) % tour.size()];
        distance += calculateDistance(cities[from], cities[to]);
    }
    return distance;
}

void SimulatedAnnealingSolver::solve() {
    // Initialize tour (0, 1, 2, ..., n-1)
    std::vector<int> currentTour(cities.size());
    std::iota(currentTour.begin(), currentTour.end(), 0);
    std::shuffle(currentTour.begin(), currentTour.end(), rng);

    double currentDistance = calculateTourDistance(currentTour);
    bestTour               = currentTour;
    bestDistance           = currentDistance;

    // Track statistics for debugging
    double initialDistance  = currentDistance;
    int    improvementCount = 0;

    double temp = initialTemp;

    for (int iter = 0; iter < maxIterations; ++iter) {
        std::vector<int> neighbor = currentTour;

        // Randomly select a mutation strategy
        std::uniform_int_distribution<int> dist(0, mutationStrategies.size() - 1);
        int                                selectedMutation = dist(rng);
        mutationStrategies[selectedMutation]->mutate(neighbor, rng);

        double neighborDistance = calculateTourDistance(neighbor);

        if (neighborDistance < currentDistance ||
            std::exp((currentDistance - neighborDistance) / temp) >
                std::uniform_real_distribution<double>(0.0, 1.0)(rng)) {
            currentTour     = neighbor;
            currentDistance = neighborDistance;

            if (currentDistance < bestDistance) {
                bestTour     = currentTour;
                bestDistance = currentDistance;
                ++improvementCount;
            }
        }

        temp *= coolingRate;
    }

    std::cout << "Initial distance: " << initialDistance << std::endl;
    std::cout << "Best distance: " << bestDistance << std::endl;
    std::cout << "Number of improvements: " << improvementCount << std::endl;
}

const std::vector<int>& SimulatedAnnealingSolver::getBestTour() const {
    return bestTour;
}

double SimulatedAnnealingSolver::getBestDistance() const {
    return bestDistance;
}
