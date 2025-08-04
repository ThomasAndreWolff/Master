#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace data {

/**
 * Represents a stop point in the network
 */
struct StopPoint {
    int         id;
    std::string code;
    std::string name;
    int         vehicleCapacityForCharging;

    StopPoint() = default;
    StopPoint(int id, const std::string& code, const std::string& name, int capacity)
      : id(id), code(code), name(name), vehicleCapacityForCharging(capacity) {}
};

/**
 * Represents a vehicle type
 */
struct VehicleType {
    int         id;
    std::string code;
    std::string name;
    double      vehicleCost;
    double      kmCost;
    double      hourCost;
    int         capacity;

    VehicleType() = default;
    VehicleType(int                id,
                const std::string& code,
                const std::string& name,
                double             vehCost,
                double             kmCost,
                double             hCost,
                int                cap)
      : id(id),
        code(code),
        name(name),
        vehicleCost(vehCost),
        kmCost(kmCost),
        hourCost(hCost),
        capacity(cap) {}
};

/**
 * Represents a vehicle type group
 */
struct VehicleTypeGroup {
    int         id;
    std::string code;
    std::string name;

    VehicleTypeGroup() = default;
    VehicleTypeGroup(int id, const std::string& code, const std::string& name)
      : id(id), code(code), name(name) {}
};

/**
 * Represents a service journey (trip)
 */
struct ServiceJourney {
    int    id;
    int    lineId;
    int    fromStopId;
    int    toStopId;
    int    depTime; // seconds from start of day
    int    arrTime; // seconds from start of day
    int    minAheadTime;
    int    minLayoverTime;
    int    vehTypeGroupId;
    int    maxShiftBackwardSeconds;
    int    maxShiftForwardSeconds;
    double distance;

    ServiceJourney() = default;
};

/**
 * Represents dead run time between stops
 */
struct DeadRunTime {
    int    fromStopId;
    int    toStopId;
    double distance;
    int    runTime; // seconds

    DeadRunTime() = default;
    DeadRunTime(int from, int to, double dist, int time)
      : fromStopId(from), toStopId(to), distance(dist), runTime(time) {}
};

/**
 * Vehicle capacity constraints at stop points
 */
struct VehicleCapacityAtStop {
    int vehicleTypeId;
    int stopPointId;
    int minCapacity;
    int maxCapacity;

    VehicleCapacityAtStop() = default;
    VehicleCapacityAtStop(int vehType, int stop, int minCap, int maxCap)
      : vehicleTypeId(vehType), stopPointId(stop), minCapacity(minCap), maxCapacity(maxCap) {}
};

/**
 * Mapping between vehicle types and vehicle type groups
 */
struct VehicleTypeToGroup {
    int id;
    int vehicleTypeId;
    int vehicleTypeGroupId;

    VehicleTypeToGroup() = default;
    VehicleTypeToGroup(int id, int vehType, int vehGroup)
      : id(id), vehicleTypeId(vehType), vehicleTypeGroupId(vehGroup) {}
};

/**
 * Represents a vehicle block/tour
 */
struct VehicleBlock {
    int              vehicleTypeId;
    int              depotStopId;
    std::vector<int> serviceJourneyIds;
    double           totalCost;
    double           totalDistance;
    int              totalTime;

    VehicleBlock()
      : vehicleTypeId(-1), depotStopId(-1), totalCost(0.0), totalDistance(0.0), totalTime(0) {}
};

/**
 * Complete solution for the MDVSP
 */
struct MDVSPGreedySolution {
    std::vector<VehicleBlock> blocks;
    double                    totalObjectiveCost;
    int                       totalVehiclesUsed;
    bool                      isFeasible;
    double                    solutionTime;

    MDVSPGreedySolution()
      : totalObjectiveCost(0.0), totalVehiclesUsed(0), isFeasible(false), solutionTime(0.0) {}
};

} // namespace data
