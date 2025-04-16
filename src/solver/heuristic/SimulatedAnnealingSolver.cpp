#include "solver/heuristic/SimulatedAnnealingSolver.h"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>

SimulatedAnnealingSolver::SimulatedAnnealingSolver(const std::vector<std::pair<double, double>> &cities, double initialTemp, double coolingRate, int maxIterations, std::shared_ptr<MutationStrategy> mutationStrategy)
    : cities(cities), bestDistance(std::numeric_limits<double>::max()), initialTemp(initialTemp), coolingRate(coolingRate), maxIterations(maxIterations), mutationStrategy(std::move(mutationStrategy))
{
    rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
}

double SimulatedAnnealingSolver::calculateDistance(const std::pair<double, double> &a, const std::pair<double, double> &b)
{
    return std::sqrt(std::pow(a.first - b.first, 2) + std::pow(a.second - b.second, 2));
}

double SimulatedAnnealingSolver::calculateTourDistance(const std::vector<int> &tour)
{
    double distance = 0.0;
    for (size_t i = 0; i < tour.size(); ++i)
    {
        int from = tour[i];
        int to = tour[(i + 1) % tour.size()];
        distance += calculateDistance(cities[from], cities[to]);
    }
    return distance;
}

void SimulatedAnnealingSolver::solve()
{
    std::vector<int> currentTour(cities.size());
    std::iota(currentTour.begin(), currentTour.end(), 0);
    std::shuffle(currentTour.begin(), currentTour.end(), rng);

    double currentDistance = calculateTourDistance(currentTour);
    bestTour = currentTour;
    bestDistance = currentDistance;

    double temp = initialTemp;

    for (int iter = 0; iter < maxIterations; ++iter)
    {
        std::vector<int> neighbor = currentTour;
        mutationStrategy->mutate(neighbor, rng);

        double neighborDistance = calculateTourDistance(neighbor);

        if (neighborDistance < currentDistance || std::exp((currentDistance - neighborDistance) / temp) > std::uniform_real_distribution<double>(0.0, 1.0)(rng))
        {
            currentTour = neighbor;
            currentDistance = neighborDistance;

            if (currentDistance < bestDistance)
            {
                bestTour = currentTour;
                bestDistance = currentDistance;
            }
        }

        temp *= coolingRate;
    }
}

const std::vector<int> &SimulatedAnnealingSolver::getBestTour() const
{
    return bestTour;
}

double SimulatedAnnealingSolver::getBestDistance() const
{
    return bestDistance;
}
