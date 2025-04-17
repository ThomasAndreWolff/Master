#ifndef TSPLIBREADER_H
#define TSPLIBREADER_H

#include <string>
#include <utility>
#include <vector>

class TSPLibReader {
  public:
    explicit TSPLibReader(const std::string& filePath);
    std::vector<std::pair<double, double>> readData();

  private:
    std::string filePath;
};

#endif // TSPLIBREADER_H
