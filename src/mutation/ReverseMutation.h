#ifndef REVERSE_MUTATION_H
#define REVERSE_MUTATION_H

#include "mutation/MutationStrategy.h"

class ReverseMutation : public MutationStrategy {
  public:
    ReverseMutation()           = default;
    ~ReverseMutation() override = default; // Virtueller Destruktor definiert
    void mutate(std::vector<int>& tour, std::mt19937& rng) const override;
};

#endif // REVERSE_MUTATION_H
