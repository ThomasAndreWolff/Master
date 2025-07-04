#pragma once

namespace GeneticAlgorithmConfig {
constexpr int    logInterval       = 100;
constexpr int    max_generations   = 1000;
constexpr int    population_size   = 200;
constexpr double crossover_rate    = 0.7;
constexpr double mutation_rate     = 0.3;
constexpr int    mutation_strength = 3; // Anzahl Inversionen pro Mutation
} // namespace GeneticAlgorithmConfig
