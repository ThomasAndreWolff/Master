#include "mutation/SwapMutation.h"
#include <algorithm>
#include <random>

void SwapMutation::mutate(std::vector<int>& tour, std::mt19937& rng) const {
    std::uniform_int_distribution<int> dist(0, tour.size() - 1);
    int                                i = dist(rng);
    int                                j = dist(rng);
    std::swap(tour[i], tour[j]);
}
