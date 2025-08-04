#pragma once

#include "data/DataStructures.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace data {

/**
 * Handles loading and managing all MDVSP data from CSV files
 */
class DataLoader {
  public:
    /**
     * Constructor
     * @param dataDirectory Path to directory containing CSV files
     */
    explicit DataLoader(const std::string& dataDirectory);

    /**
     * Load all data from CSV files
     * @return true if successful, false otherwise
     */
    bool loadAll();

    /**
     * Get all service journeys
     */
    const std::vector<ServiceJourney>& getServiceJourneys() const { return serviceJourneys_; }

    /**
     * Get all vehicle types
     */
    const std::vector<VehicleType>& getVehicleTypes() const { return vehicleTypes_; }

    /**
     * Get all stop points
     */
    const std::vector<StopPoint>& getStopPoints() const { return stopPoints_; }

    /**
     * Get all vehicle type groups
     */
    const std::vector<VehicleTypeGroup>& getVehicleTypeGroups() const { return vehicleTypeGroups_; }

    /**
     * Get dead run time between two stops
     * @param fromStop Source stop ID
     * @param toStop Destination stop ID
     * @return Dead run time in seconds, -1 if not found
     */
    int getDeadRunTime(int fromStop, int toStop) const;

    /**
     * Get dead run distance between two stops
     */
    double getDeadRunDistance(int fromStop, int toStop) const;

    /**
     * Check if vehicle type is compatible with vehicle type group
     */
    bool isVehicleTypeCompatible(int vehicleTypeId, int vehicleTypeGroupId) const;

    /**
     * Get vehicle types compatible with a vehicle type group
     */
    std::vector<int> getCompatibleVehicleTypes(int vehicleTypeGroupId) const;

    /**
     * Get depot stop points (stops with vehicle capacity > 0)
     */
    std::vector<int> getDepotStops() const;

    /**
     * Get vehicle capacity at a specific stop
     */
    int getVehicleCapacityAtStop(int vehicleTypeId, int stopId) const;

    /**
     * Get stop point by ID
     */
    const StopPoint* getStopPoint(int id) const;

    /**
     * Get vehicle type by ID
     */
    const VehicleType* getVehicleType(int id) const;

    /**
     * Check if data is loaded successfully
     */
    bool isLoaded() const { return loaded_; }

  private:
    std::string dataDirectory_;
    bool        loaded_;

    // Data containers
    std::vector<ServiceJourney>        serviceJourneys_;
    std::vector<VehicleType>           vehicleTypes_;
    std::vector<StopPoint>             stopPoints_;
    std::vector<VehicleTypeGroup>      vehicleTypeGroups_;
    std::vector<VehicleCapacityAtStop> vehicleCapacities_;
    std::vector<VehicleTypeToGroup>    vehicleTypeToGroups_;

    // Fast lookup maps
    std::unordered_map<std::string, int>      deadRunTimes_;     // "fromStop_toStop" -> time
    std::unordered_map<std::string, double>   deadRunDistances_; // "fromStop_toStop" -> distance
    std::unordered_map<int, std::vector<int>> vehicleTypeToGroupMap_; // vehTypeGroup -> [vehTypes]
    std::unordered_map<std::string, int>      vehicleCapacityMap_;    // "vehType_stop" -> capacity
    std::unordered_map<int, StopPoint>        stopPointMap_;
    std::unordered_map<int, VehicleType>      vehicleTypeMap_;

    // Loading methods
    bool loadServiceJourneys();
    bool loadVehicleTypes();
    bool loadStopPoints();
    bool loadVehicleTypeGroups();
    bool loadDeadRunTimes();
    bool loadVehicleCapacities();
    bool loadVehicleTypeToGroups();

    // Helper methods
    std::vector<std::string> parseCsvLine(const std::string& line);
    int                      timeStringToSeconds(const std::string& timeStr);
    std::string              makeKey(int from, int to) const;

    void buildLookupMaps();
};

} // namespace data
