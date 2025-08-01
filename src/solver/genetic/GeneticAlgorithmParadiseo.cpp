#include "GeneticAlgorithmParadiseo.h"
#include <cmath>
#include <eo>
#include <numeric>
#include <random>
#include <vector>
#include "logger/TSPResultLogger.h"
#include "settings/GeneticAlgorithmConfig.h"

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

    // ParadisEO: explizit Minimierungsproblem deklarieren
    static bool minimising_fitness() { return true; }
};

GeneticAlgorithmParadiseo::GeneticAlgorithmParadiseo(const std::vector<std::pair<double, double>>& cities)
  : cities(cities), bestDistance(std::numeric_limits<double>::max()) {}

void GeneticAlgorithmParadiseo::solve() {
    // Logger für bestes Individuum
    TSPResultLogger logger("database/tsp_results.db");
    // Globale Parameter verwenden
    int logInterval     = GeneticAlgorithmConfig::logInterval;
    int max_generations = GeneticAlgorithmConfig::max_generations;
    int population_size = GeneticAlgorithmConfig::population_size;

    // Zufallszahlengenerator einmalig anlegen
    std::random_device rd;
    std::mt19937       rng(rd());

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

    // Define the mutation operator (Reverse Mutation, mehrfach pro Individuum)
    class TSPMutation : public eoMonOp<TSPVector> {
      public:
        TSPMutation(std::mt19937& rng, int n = 3) : rng_(rng), n_(n) {}
        bool operator()(TSPVector& solution) {
            if (solution.size() < 2)
                return false;
            std::uniform_int_distribution<int> dist(0, solution.size() - 1);
            for (int k = 0; k < n_; ++k) {
                int i = dist(rng_);
                int j = dist(rng_);
                if (i > j)
                    std::swap(i, j);
                std::reverse(solution.begin() + i, solution.begin() + j + 1);
            }
            return true;
        }

      private:
        std::mt19937& rng_;
        int           n_;
    };

    // Define the crossover operator (Order Crossover, OX)
    class TSPCrossover : public eoQuadOp<TSPVector> {
      public:
        TSPCrossover(std::mt19937& rng) : rng_(rng) {}
        bool operator()(TSPVector& parent1, TSPVector& parent2) {
            if (parent1.size() < 2)
                return false;
            std::uniform_int_distribution<int> dist(0, parent1.size() - 1);
            int                                size  = parent1.size();
            int                                start = dist(rng_);
            int                                end   = dist(rng_);
            if (start > end)
                std::swap(start, end);

            // Create offspring copies
            TSPVector offspring1 = parent1;
            TSPVector offspring2 = parent2;

            // Helper lambda for OX
            auto ox = [&](const TSPVector& p1, const TSPVector& p2, TSPVector& child) {
                std::vector<bool> inChild(size, false);
                // Copy slice from p1
                for (int i = start; i <= end; ++i) {
                    child[i]       = p1[i];
                    inChild[p1[i]] = true;
                }
                // Fill remaining from p2
                int curr  = (end + 1) % size;
                int p2idx = (end + 1) % size;
                while (curr != start) {
                    while (inChild[p2[p2idx]]) {
                        p2idx = (p2idx + 1) % size;
                    }
                    child[curr]        = p2[p2idx];
                    inChild[p2[p2idx]] = true;
                    curr               = (curr + 1) % size;
                }
            };

            ox(parent1, parent2, offspring1);
            ox(parent2, parent1, offspring2);
            parent1 = offspring1;
            parent2 = offspring2;
            return true;
        }

      private:
        std::mt19937& rng_;
    };

    // Initialize the problem, mutation, and crossover
    TSPProblem   problem(cities);
    TSPMutation  mutation(rng, GeneticAlgorithmConfig::mutation_strength);
    TSPCrossover crossover(rng);

    // Define the solution representation
    TSPVector solution(cities.size());
    std::iota(solution.begin(), solution.end(), 0);

    // Define the population with valid individuals
    eoPop<TSPVector> population;
    for (int i = 0; i < population_size; ++i) {
        TSPVector individual(cities.size());
        std::iota(individual.begin(), individual.end(), 0);
        std::shuffle(individual.begin(), individual.end(), rng);
        population.push_back(individual);
    }

    // Hilfsfunktion: Prüft, ob ein TSPVector eine gültige Permutation ist
    auto isValidTour = [](const TSPVector& tour) {
        std::vector<bool> seen(tour.size(), false);
        for (int city : tour) {
            if (city < 0 || city >= (int) tour.size() || seen[city])
                return false;
            seen[city] = true;
        }
        return true;
    };

    // Manuelle GA-Schleife für Logging
    eoDetTournamentSelect<TSPVector> selector(2);
    eoSGATransform<TSPVector>        transform(crossover,
                                        GeneticAlgorithmConfig::crossover_rate,
                                        mutation,
                                        GeneticAlgorithmConfig::mutation_rate);
    // Globale Best-Variable
    TSPVector globalBest;
    double    globalBestFitness = std::numeric_limits<double>::max();

    for (int gen = 0; gen < max_generations; ++gen) {
        // Evaluate population
        for (auto& ind : population)
            problem(ind);
        // Logging
        if (gen % logInterval == 0) {
            const TSPVector& best = *std::min_element(
                population.begin(), population.end(), [](const TSPVector& a, const TSPVector& b) {
                    return a.fitness < b.fitness;
                });
            std::cout << "Generation: " << gen << ", Best Fitness: " << best.fitness << std::endl;
            if (!isValidTour(best))
                std::cout << "WARNUNG: Ungültige Tour!" << std::endl;
            logger.logBestIndividual(gen, best.fitness, best);
        }
        // Selection
        eoPop<TSPVector> offspring;
        // Elitismus: bestes Individuum übernehmen (explizite Kopie!)
        const TSPVector& elite = *std::min_element(
            population.begin(), population.end(), [](const TSPVector& a, const TSPVector& b) {
                return a.fitness < b.fitness;
            });
        offspring.push_back(elite);
        for (int i = 1; i < population_size; ++i) {
            const TSPVector& selected = selector(population);
            offspring.push_back(selected);
        }
        // Variation
        if (offspring.size() > 1) {
            eoPop<TSPVector> nonElite;
            for (size_t i = 1; i < offspring.size(); ++i)
                nonElite.push_back(offspring[i]);
            transform(nonElite);
            for (size_t i = 1; i < offspring.size(); ++i)
                offspring[i] = nonElite[i - 1];
        }
        // Nach Variation: Fitness neu berechnen (Elite bleibt unangetastet)
        for (size_t i = 1; i < offspring.size(); ++i)
            problem(offspring[i]);
        // Nach Variation: Prüfen auf gültige Touren
        for (const auto& ind : offspring) {
            if (!isValidTour(ind)) {
                std::cout << "WARNUNG: Ungültige Tour nach Variation!" << std::endl;
            }
        }
        // Diversitäts-Check: Doppelte Individuen durch neue Zufalls-Individuen ersetzen
        auto isDuplicate = [](const TSPVector& a, const TSPVector& b) {
            return std::equal(a.begin(), a.end(), b.begin());
        };
        for (size_t i = 1; i < offspring.size(); ++i) {
            for (size_t j = 0; j < i; ++j) {
                if (isDuplicate(offspring[i], offspring[j])) {
                    // Ersetze Duplikat durch neue Zufalls-Tour
                    std::iota(offspring[i].begin(), offspring[i].end(), 0);
                    std::shuffle(offspring[i].begin(), offspring[i].end(), rng);
                    problem(offspring[i]);
                    break;
                }
            }
        }
        // Globales Bestes aktualisieren
        const TSPVector& currentBest = *std::min_element(
            offspring.begin(), offspring.end(), [](const TSPVector& a, const TSPVector& b) {
                return a.fitness < b.fitness;
            });
        if (currentBest.fitness < globalBestFitness) {
            globalBest        = currentBest;
            globalBestFitness = currentBest.fitness;
        }
        population = offspring;
    }
    // Nach letzter Generation bestes Individuum speichern
    logger.logBestIndividual(max_generations, globalBestFitness, globalBest);
    bestTour     = globalBest;
    bestDistance = globalBestFitness;
}

double GeneticAlgorithmParadiseo::getBestDistance() const {
    return bestDistance;
}

std::vector<int> GeneticAlgorithmParadiseo::getBestTour() const {
    return bestTour;
}