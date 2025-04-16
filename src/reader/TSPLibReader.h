#ifndef TSPLIBREADER_H
#define TSPLIBREADER_H

#include <vector>
#include <string>
#include <utility>

class TSPLibReader
{
public:
    explicit TSPLibReader(const std::string &filePath);
    std::vector<std::pair<double, double>> readData();

private:
    std::string filePath;
};

#endif // TSPLIBREADER_H
