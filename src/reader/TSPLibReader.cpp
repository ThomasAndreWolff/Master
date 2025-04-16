#include "TSPLibReader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

TSPLibReader::TSPLibReader(const std::string &filePath) : filePath(filePath) {}

std::vector<std::pair<double, double>> TSPLibReader::readData()
{
    std::vector<std::pair<double, double>> cities;
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Unable to open file: " + filePath);
    }

    std::string line;
    bool nodeSection = false;
    while (std::getline(file, line))
    {
        if (line == "NODE_COORD_SECTION")
        {
            nodeSection = true;
            continue;
        }
        if (line == "EOF")
            break;

        if (nodeSection)
        {
            std::istringstream iss(line);
            int id;
            double x, y;
            iss >> id >> x >> y;
            cities.emplace_back(x, y);
        }
    }

    file.close();
    return cities;
}