#include "mutation/ReverseMutation.h"
#include <algorithm>
#include <random>

void ReverseMutation::mutate(std::vector<int> &tour, std::mt19937 &rng) const
{
    std::uniform_int_distribution<int> dist(0, tour.size() - 1);
    int i = dist(rng);
    int j = dist(rng);
    if (i > j)
        std::swap(i, j);
    std::reverse(tour.begin() + i, tour.begin() + j + 1);
}
