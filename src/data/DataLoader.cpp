#include "data/DataLoader.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace data {

DataLoader::DataLoader(const std::string& dataDirectory)
  : dataDirectory_(dataDirectory), loaded_(false) {
    // Ensure directory ends with /
    if (!dataDirectory_.empty() && dataDirectory_.back() != '/') {
        dataDirectory_ += '/';
    }
}

bool DataLoader::loadAll() {
    std::cout << "Loading MDVSP data from: " << dataDirectory_ << std::endl;

    if (!loadStopPoints()) {
        std::cerr << "Failed to load stop points" << std::endl;
        return false;
    }

    if (!loadVehicleTypes()) {
        std::cerr << "Failed to load vehicle types" << std::endl;
        return false;
    }

    if (!loadVehicleTypeGroups()) {
        std::cerr << "Failed to load vehicle type groups" << std::endl;
        return false;
    }

    if (!loadServiceJourneys()) {
        std::cerr << "Failed to load service journeys" << std::endl;
        return false;
    }

    if (!loadDeadRunTimes()) {
        std::cerr << "Failed to load dead run times" << std::endl;
        return false;
    }

    if (!loadVehicleCapacities()) {
        std::cerr << "Failed to load vehicle capacities" << std::endl;
        return false;
    }

    if (!loadVehicleTypeToGroups()) {
        std::cerr << "Failed to load vehicle type to groups mapping" << std::endl;
        return false;
    }

    buildLookupMaps();
    loaded_ = true;

    std::cout << "Successfully loaded MDVSP data:" << std::endl;
    std::cout << "  - Service journeys: " << serviceJourneys_.size() << std::endl;
    std::cout << "  - Vehicle types: " << vehicleTypes_.size() << std::endl;
    std::cout << "  - Stop points: " << stopPoints_.size() << std::endl;
    std::cout << "  - Vehicle type groups: " << vehicleTypeGroups_.size() << std::endl;

    return true;
}

bool DataLoader::loadServiceJourneys() {
    std::ifstream file(dataDirectory_ + "servicejourney.csv");
    if (!file.is_open()) {
        std::cerr << "Cannot open servicejourney.csv" << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        auto fields = parseCsvLine(line);
        if (fields.size() < 12)
            continue;

        ServiceJourney journey;
        journey.id                      = std::stoi(fields[0]);
        journey.lineId                  = std::stoi(fields[1]);
        journey.fromStopId              = std::stoi(fields[2]);
        journey.toStopId                = std::stoi(fields[3]);
        journey.depTime                 = timeStringToSeconds(fields[4]);
        journey.arrTime                 = timeStringToSeconds(fields[5]);
        journey.minAheadTime            = std::stoi(fields[6]);
        journey.minLayoverTime          = std::stoi(fields[7]);
        journey.vehTypeGroupId          = std::stoi(fields[8]);
        journey.maxShiftBackwardSeconds = std::stoi(fields[9]);
        journey.maxShiftForwardSeconds  = std::stoi(fields[10]);
        journey.distance                = std::stod(fields[11]);

        serviceJourneys_.push_back(journey);
    }

    return true;
}

bool DataLoader::loadVehicleTypes() {
    std::ifstream file(dataDirectory_ + "vehicletype.csv");
    if (!file.is_open()) {
        std::cerr << "Cannot open vehicletype.csv" << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        auto fields = parseCsvLine(line);
        if (fields.size() < 7)
            continue;

        VehicleType vehType;
        vehType.id          = std::stoi(fields[0]);
        vehType.code        = fields[1];
        vehType.name        = fields[2];
        vehType.vehicleCost = std::stod(fields[3]);
        vehType.kmCost      = std::stod(fields[4]);
        vehType.hourCost    = std::stod(fields[5]);
        vehType.capacity    = std::stoi(fields[6]);

        vehicleTypes_.push_back(vehType);
    }

    return true;
}

bool DataLoader::loadStopPoints() {
    std::ifstream file(dataDirectory_ + "stoppoint.csv");
    if (!file.is_open()) {
        std::cerr << "Cannot open stoppoint.csv" << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        auto fields = parseCsvLine(line);
        if (fields.size() < 4)
            continue;

        StopPoint stop;
        stop.id                         = std::stoi(fields[0]);
        stop.code                       = fields[1];
        stop.name                       = fields[2];
        stop.vehicleCapacityForCharging = std::stoi(fields[3]);

        stopPoints_.push_back(stop);
    }

    return true;
}

bool DataLoader::loadVehicleTypeGroups() {
    std::ifstream file(dataDirectory_ + "vehicletypegroup.csv");
    if (!file.is_open()) {
        std::cerr << "Cannot open vehicletypegroup.csv" << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        auto fields = parseCsvLine(line);
        if (fields.size() < 3)
            continue;

        VehicleTypeGroup group;
        group.id   = std::stoi(fields[0]);
        group.code = fields[1];
        group.name = fields[2];

        vehicleTypeGroups_.push_back(group);
    }

    return true;
}

bool DataLoader::loadDeadRunTimes() {
    std::ifstream file(dataDirectory_ + "deadruntime.csv");
    if (!file.is_open()) {
        std::cerr << "Cannot open deadruntime.csv" << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        auto fields = parseCsvLine(line);
        if (fields.size() < 4)
            continue;

        int    fromStop = std::stoi(fields[0]);
        int    toStop   = std::stoi(fields[1]);
        double distance = std::stod(fields[2]);
        int    runTime  = std::stoi(fields[3]);

        std::string key        = makeKey(fromStop, toStop);
        deadRunTimes_[key]     = runTime;
        deadRunDistances_[key] = distance;
    }

    return true;
}

bool DataLoader::loadVehicleCapacities() {
    std::ifstream file(dataDirectory_ + "vehtypecaptostoppoint.csv");
    if (!file.is_open()) {
        std::cerr << "Cannot open vehtypecaptostoppoint.csv" << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        auto fields = parseCsvLine(line);
        if (fields.size() < 4)
            continue;

        VehicleCapacityAtStop capacity;
        capacity.vehicleTypeId = std::stoi(fields[0]);
        capacity.stopPointId   = std::stoi(fields[1]);
        capacity.minCapacity   = std::stoi(fields[2]);
        capacity.maxCapacity   = std::stoi(fields[3]);

        vehicleCapacities_.push_back(capacity);
    }

    return true;
}

bool DataLoader::loadVehicleTypeToGroups() {
    std::ifstream file(dataDirectory_ + "vehtypetovehtypegroup.csv");
    if (!file.is_open()) {
        std::cerr << "Cannot open vehtypetovehtypegroup.csv" << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        auto fields = parseCsvLine(line);
        if (fields.size() < 3)
            continue;

        VehicleTypeToGroup mapping;
        mapping.id                 = std::stoi(fields[0]);
        mapping.vehicleTypeId      = std::stoi(fields[1]);
        mapping.vehicleTypeGroupId = std::stoi(fields[2]);

        vehicleTypeToGroups_.push_back(mapping);
    }

    return true;
}

std::vector<std::string> DataLoader::parseCsvLine(const std::string& line) {
    std::vector<std::string> result;
    std::stringstream        ss(line);
    std::string              field;

    while (std::getline(ss, field, ';')) {
        result.push_back(field);
    }

    return result;
}

int DataLoader::timeStringToSeconds(const std::string& timeStr) {
    // Format: "000:03:48:00" -> seconds
    if (timeStr.length() < 11)
        return 0;

    int hours   = std::stoi(timeStr.substr(4, 2));
    int minutes = std::stoi(timeStr.substr(7, 2));
    int seconds = std::stoi(timeStr.substr(10, 2));

    return hours * 3600 + minutes * 60 + seconds;
}

std::string DataLoader::makeKey(int from, int to) const {
    return std::to_string(from) + "_" + std::to_string(to);
}

void DataLoader::buildLookupMaps() {
    // Build vehicle type to group mapping
    for (const auto& mapping : vehicleTypeToGroups_) {
        vehicleTypeToGroupMap_[mapping.vehicleTypeGroupId].push_back(mapping.vehicleTypeId);
    }

    // Build vehicle capacity mapping
    for (const auto& capacity : vehicleCapacities_) {
        std::string key =
            std::to_string(capacity.vehicleTypeId) + "_" + std::to_string(capacity.stopPointId);
        vehicleCapacityMap_[key] = capacity.maxCapacity;
    }

    // Build stop point and vehicle type maps
    for (const auto& stop : stopPoints_) {
        stopPointMap_[stop.id] = stop;
    }

    for (const auto& vehType : vehicleTypes_) {
        vehicleTypeMap_[vehType.id] = vehType;
    }
}

int DataLoader::getDeadRunTime(int fromStop, int toStop) const {
    std::string key = makeKey(fromStop, toStop);
    auto        it  = deadRunTimes_.find(key);
    return (it != deadRunTimes_.end()) ? it->second : -1;
}

double DataLoader::getDeadRunDistance(int fromStop, int toStop) const {
    std::string key = makeKey(fromStop, toStop);
    auto        it  = deadRunDistances_.find(key);
    return (it != deadRunDistances_.end()) ? it->second : -1.0;
}

bool DataLoader::isVehicleTypeCompatible(int vehicleTypeId, int vehicleTypeGroupId) const {
    auto it = vehicleTypeToGroupMap_.find(vehicleTypeGroupId);
    if (it == vehicleTypeToGroupMap_.end())
        return false;

    const auto& compatibleTypes = it->second;
    return std::find(compatibleTypes.begin(), compatibleTypes.end(), vehicleTypeId) !=
           compatibleTypes.end();
}

std::vector<int> DataLoader::getCompatibleVehicleTypes(int vehicleTypeGroupId) const {
    auto it = vehicleTypeToGroupMap_.find(vehicleTypeGroupId);
    return (it != vehicleTypeToGroupMap_.end()) ? it->second : std::vector<int>();
}

std::vector<int> DataLoader::getDepotStops() const {
    std::vector<int> depots;

    // Get depots from vehicle capacity constraints (VEHTYPECAPTOSTOPPOINT)
    for (const auto& capacity : vehicleCapacities_) {
        if (capacity.maxCapacity > 0) {
            // Check if this stop is not already in the list
            if (std::find(depots.begin(), depots.end(), capacity.stopPointId) == depots.end()) {
                depots.push_back(capacity.stopPointId);
            }
        }
    }

    return depots;
}

int DataLoader::getVehicleCapacityAtStop(int vehicleTypeId, int stopId) const {
    std::string key = std::to_string(vehicleTypeId) + "_" + std::to_string(stopId);
    auto        it  = vehicleCapacityMap_.find(key);
    return (it != vehicleCapacityMap_.end()) ? it->second : 0;
}

const StopPoint* DataLoader::getStopPoint(int id) const {
    auto it = stopPointMap_.find(id);
    return (it != stopPointMap_.end()) ? &it->second : nullptr;
}

const VehicleType* DataLoader::getVehicleType(int id) const {
    auto it = vehicleTypeMap_.find(id);
    return (it != vehicleTypeMap_.end()) ? &it->second : nullptr;
}

} // namespace data
