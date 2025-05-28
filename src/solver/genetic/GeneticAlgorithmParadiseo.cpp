#include "GeneticAlgorithmParadiseo.h"
#include <cmath>
#include <eo>
#include <numeric>
#include <random>
#include <vector>

// Define a custom solution type with a fitness member
struct TSPVector : public std::vector<int> {
    using Fitness = double; // Required by eoEvalFunc and eoEasyEA
    Fitness fitness;

    TSPVector(size_t size) : std::vector<int>(size), fitness(0.0) {}

    // Add default constructor
    TSPVector() : std::vector<int>(), fitness(0.0) {}

    // Add invalidate method
    void invalidate() { fitness = std::numeric_limits<Fitness>::max(); }

    // Add operator< for sorting
    bool operator<(const TSPVector& other) const { return fitness < other.fitness; }

    // Add operator<< for output streaming
    friend std::ostream& operator<<(std::ostream& os, const TSPVector& vec) {
        for (int val : vec) {
            os << val << " ";
        }
        os << "| Fitness: " << vec.fitness;
        return os;
    }

    // Add readFrom method
    void readFrom(std::istream& is) {
        for (auto& val : *this) {
            is >> val;
        }
        is >> fitness;
    }
};

GeneticAlgorithmParadiseo::GeneticAlgorithmParadiseo(const std::vector<std::pair<double, double>>& cities)
  : cities(cities), bestDistance(std::numeric_limits<double>::max()) {}

void GeneticAlgorithmParadiseo::solve() {
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

    // Define the crossover operator
    class TSPCrossover : public eoQuadOp<TSPVector> {
      public:
        bool operator()(TSPVector& parent1, TSPVector& parent2) {
            std::random_device                 rd;
            std::mt19937                       rng(rd());
            std::uniform_int_distribution<int> dist(0, parent1.size() - 1);

            int crossoverPoint = dist(rng);
            for (size_t i = 0; i < crossoverPoint; ++i) {
                std::swap(parent1[i], parent2[i]);
            }
            return true;
        }
    };

    // Initialize the problem, mutation, and crossover
    TSPProblem   problem(cities);
    TSPMutation  mutation;
    TSPCrossover crossover;

    // Define the solution representation
    TSPVector solution(cities.size());
    std::iota(solution.begin(), solution.end(), 0);

    // Define the population with valid individuals
    eoPop<TSPVector> population;
    for (int i = 0; i < 100; ++i) {
        TSPVector individual(cities.size());
        std::iota(individual.begin(), individual.end(), 0);
        std::random_device rd;
        std::mt19937       rng(rd());
        std::shuffle(individual.begin(), individual.end(), rng);
        population.push_back(individual);
    }

    // Define the genetic algorithm
    eoGenContinue<TSPVector>  continuator(1000); // Stop after 1000 generations
    eoSGATransform<TSPVector> transform(crossover, 0.7, mutation, 0.3); // 70% crossover, 30%
                                                                        // mutation
    eoDetTournamentSelect<TSPVector> selector(2); // Tournament selection with size 2
    eoSGA<TSPVector> geneticAlgorithm(selector, crossover, 0.7, mutation, 0.3, problem, continuator);

    // Run the genetic algorithm
    geneticAlgorithm(population);

    // Store the best solution
    bestTour     = population.best_element();
    bestDistance = population.best_element().fitness;
}

double GeneticAlgorithmParadiseo::getBestDistance() const {
    return bestDistance;
}

std::vector<int> GeneticAlgorithmParadiseo::getBestTour() const {
    return bestTour;
}