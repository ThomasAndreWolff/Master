#include "SimulatedAnnealingParadiseo.h"
#include <cmath>
#include <eo>
#include <numeric>
#include <random>

// Define a custom solution type with a fitness member
struct TSPVector : public std::vector<int> {
    using Fitness = double; // Required by eoEvalFunc and eoEasyEA
    Fitness fitness;

    TSPVector(size_t size) : std::vector<int>(size), fitness(0.0) {}
};

SimulatedAnnealingParadiseo::SimulatedAnnealingParadiseo(
    const std::vector<std::pair<double, double>>& cities)
  : cities(cities), bestDistance(std::numeric_limits<double>::max()) {}

void SimulatedAnnealingParadiseo::solve() {
    // Define the problem evaluation function
    class TSPProblem : public eoEvalFunc<TSPVector> {
      private:
        const std::vector<std::pair<double, double>>& cities;

        double calculateDistance(const std::pair<double, double>& a,
                                 const std::pair<double, double>& b) {
            return std::sqrt(std::pow(a.first - b.first, 2) + std::pow(a.second - b.second, 2));
        }

      public:
        TSPProblem(const std::vector<std::pair<double, double>>& cities) : cities(cities) {}

        void operator()(TSPVector& solution) {
            double distance = 0.0;
            for (size_t i = 0; i < solution.size(); ++i) {
                int from = solution[i];
                int to   = solution[(i + 1) % solution.size()];
                distance += calculateDistance(cities[from], cities[to]);
            }
            solution.fitness = distance;
        }
    };

    // Define the mutation operator
    class TSPMutation : public eoMonOp<TSPVector> {
      public:
        bool operator()(TSPVector& solution) {
            std::random_device                 rd;
            std::mt19937                       rng(rd());
            std::uniform_int_distribution<int> dist(0, solution.size() - 1);

            int i = dist(rng);
            int j = dist(rng);
            std::swap(solution[i], solution[j]);
            return true;
        }
    };

    // Initialize the problem and mutation
    TSPProblem  problem(cities);
    TSPMutation mutation;

    // Define the solution representation
    TSPVector solution(cities.size());
    std::iota(solution.begin(), solution.end(), 0);

    // Custom Simulated Annealing implementation
    double temperature   = 1000.0;
    double coolingRate   = 0.995;
    int    maxIterations = 10000;

    TSPVector currentSolution = solution;
    TSPVector bestSolution    = solution;
    problem(currentSolution);
    bestSolution.fitness = currentSolution.fitness;

    std::random_device                     rd;
    std::mt19937                           rng(rd());
    std::uniform_real_distribution<double> acceptanceDist(0.0, 1.0);

    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        TSPVector newSolution = currentSolution;
        mutation(newSolution);
        problem(newSolution);

        double deltaFitness = newSolution.fitness - currentSolution.fitness;
        if (deltaFitness < 0 || acceptanceDist(rng) < std::exp(-deltaFitness / temperature)) {
            currentSolution = newSolution;
        }

        if (currentSolution.fitness < bestSolution.fitness) {
            bestSolution = currentSolution;
        }

        temperature *= coolingRate;
    }

    // Store the best solution
    bestTour     = bestSolution;
    bestDistance = bestSolution.fitness;
}

double SimulatedAnnealingParadiseo::getBestDistance() const {
    return bestDistance;
}

std::vector<int> SimulatedAnnealingParadiseo::getBestTour() const {
    return bestTour;
}