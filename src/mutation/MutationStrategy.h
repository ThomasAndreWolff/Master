#ifndef MUTATION_STRATEGY_H
#define MUTATION_STRATEGY_H

#include <vector>
#include <random>

class MutationStrategy
{
public:
    virtual void mutate(std::vector<int> &tour, std::mt19937 &rng) const = 0;
    virtual ~MutationStrategy() = default;
};

#endif // MUTATION_STRATEGY_H
