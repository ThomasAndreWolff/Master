#ifndef SWAP_MUTATION_H
#define SWAP_MUTATION_H

#include "mutation/MutationStrategy.h"

class SwapMutation : public MutationStrategy {
  public:
    void mutate(std::vector<int>& tour, std::mt19937& rng) const override;
};

#endif // SWAP_MUTATION_H
