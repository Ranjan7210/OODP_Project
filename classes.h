/**
 * =============================================================================
 * FILE        : classes.h
 * PROJECT     : Bus Route Management System — Mumbai BEST
 * AUTHOR      : Member 1 (OOP Architect)
 * DESCRIPTION : All class declarations, pure virtual functions, and OOP
 *               design for the Bus Route Management System.
 *
 * OOP CONCEPTS DEMONSTRATED
 * ─────────────────────────
 *  1. ABSTRACTION    — TransportEntity is an abstract base class (pure virtuals)
 *  2. INHERITANCE    — Bus, Stop, Route inherit from TransportEntity (IS-A)
 *  3. ENCAPSULATION  — All data members are private/protected; accessed via
 *                      getters and setters only
 *  4. POLYMORPHISM   — virtual getInfo() and calculateMetric() enable runtime
 *                      dispatch through TransportEntity* pointers
 *  5. COMPOSITION    — Schedule HAS-A bus reference and route reference (HAS-A)
 *
 * CSV COLUMN STRUCTURE (agreed with Member 2)
 * ─────────────────────────────────────────────
 *  buses.csv     : id, name, busNumber, capacity, type, status, currentRouteId
 *  stops.csv     : id, name, stopCode, landmark, latitude, longitude, routeCount
 *  routes.csv    : id, name, routeNumber, startStop, endStop, totalDistance, stops
 *  schedules.csv : id, busId, routeId, departureTime, arrivalTime, days
 *
 * MENU OPTIONS (agreed with Member 2 for main.cpp)
 * ──────────────────────────────────────────────────
 *  Main  : Bus Mgmt | Stop Mgmt | Route Mgmt | Schedule Mgmt | Reports | Exit
 *  Bus   : Add | View All | Delete | Toggle Status (Active↔Maintenance)
 *  Stop  : Add | View All | Distance Between Two Stops
 *  Route : Add | View All | Search by Stop Name
 *  Sched : Add | View All | Conflict Detection
 *
 * FLASK TEMPLATE MAPPING (suggestion for Member 3)
 * ──────────────────────────────────────────────────
 *  index.html      → Dashboard: counts of buses, stops, routes, schedules
 *  buses.html      → Renders Bus::getInfo() fields in a table
 *  stops.html      → Renders Stop::getInfo() fields (lat/lon, landmark)
 *  routes.html     → Renders Route::getInfo() with stop chain
 *  schedules.html  → Renders Schedule::getInfo() (dep/arr/days)
 *  stats.html      → Statistics charts (for Member 4)
 *
 * STATISTICS CHART SUGGESTIONS (for Member 4)
 * ─────────────────────────────────────────────
 *  • Pie chart   : AC vs Non-AC bus split
 *  • Bar chart   : Seat capacity per bus (Bus::calculateMetric())
 *  • Bar chart   : Number of routes per stop (Stop::calculateMetric())
 *  • Bar chart   : Route distance comparison (Route::calculateMetric())
 *  • Line chart  : Schedule departure times across the day
 * =============================================================================
 */

#ifndef CLASSES_H
#define CLASSES_H

// ── Standard library includes ──────────────────────────────────────────────
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>

// =============================================================================
//  ABSTRACT BASE CLASS — TransportEntity
// =============================================================================
/**
 * @brief Abstract base class for all transport domain objects.
 *
 * Provides a common interface via two pure virtual methods that every
 * concrete subclass MUST implement:
 *   - getInfo()         : prints a human-readable description to stdout
 *   - calculateMetric() : returns one numeric KPI for reports/charts
 *
 * The class supplies a virtual destructor so that `delete base_ptr` on any
 * derived object calls the correct destructor chain (essential for
 * polymorphic containers such as vector<TransportEntity*>).
 *
 * Protected fields (`id`, `name`) are shared by all subclasses and
 * represent the minimum identity of any transport entity.
 */
class TransportEntity {
protected:
    int         id;    ///< Unique numeric identifier (auto-incremented)
    std::string name;  ///< Human-readable display name

public:
    // ── Constructors ──────────────────────────────────────────────────────
    TransportEntity();
    TransportEntity(int id, const std::string& name);

    // ── Pure Virtual Interface (forces override in all derived classes) ───
    /**
     * @brief Prints all fields of this entity to stdout in a formatted box.
     * @note  Must be overridden by every concrete derived class.
     */
    virtual void   getInfo()         const = 0;

    /**
     * @brief Returns the primary numeric metric for this entity.
     *
     * - Bus    → seat capacity  (used for bar/pie charts)
     * - Stop   → route count    (used for bar charts)
     * - Route  → total distance (used for bar charts)
     *
     * @return A double representing the entity's key performance indicator.
     * @note  Must be overridden by every concrete derived class.
     */
    virtual double calculateMetric() const = 0;

    // ── Virtual Destructor ────────────────────────────────────────────────
    /**
     * @brief Virtual destructor — ensures correct destruction through
     *        base-class pointers (required for polymorphic use).
     */
    virtual ~TransportEntity();

    // ── Getters (Encapsulation — read-only access to protected fields) ───
    int         getId()   const;
    std::string getName() const;

    // ── Setters (Encapsulation — controlled write access) ────────────────
    void setId(int newId);
    void setName(const std::string& newName);
};


// =============================================================================
//  CONCRETE CLASS — Bus  (IS-A TransportEntity)
// =============================================================================
/**
 * @brief Represents a single BEST bus in the fleet.
 *
 * Inherits identity (id, name) from TransportEntity and adds bus-specific
 * fields:
 *   - busNumber      : Registration plate, e.g. "MH-01-1234"
 *   - capacity       : Passenger seating capacity
 *   - type           : "AC" or "Non-AC"
 *   - status         : "Active" or "Maintenance"
 *   - currentRouteId : Route number currently assigned, e.g. "R-101"
 *
 * CSV serialisation format (toCSV):
 *   id,name,busNumber,capacity,type,status,currentRouteId
 *
 * calculateMetric() returns capacity (double) for fleet-capacity charts.
 */
class Bus : public TransportEntity {
private:
    std::string busNumber;      ///< Vehicle registration number
    int         capacity;       ///< Total passenger seating capacity
    std::string type;           ///< "AC" | "Non-AC"
    std::string status;         ///< "Active" | "Maintenance"
    std::string currentRouteId; ///< Currently assigned route ID (e.g. "R-101")

public:
    // ── Constructors ──────────────────────────────────────────────────────
    Bus();
    Bus(int id, const std::string& name, const std::string& busNumber,
        int capacity, const std::string& type, const std::string& status,
        const std::string& currentRouteId);

    // ── Overridden Virtual Functions ──────────────────────────────────────
    void   getInfo()         const override; ///< Prints bus details to stdout
    double calculateMetric() const override; ///< Returns (double)capacity

    // ── CSV Serialisation ─────────────────────────────────────────────────
    /**
     * @brief Serialises bus data as a single comma-separated string.
     * @return "id,name,busNumber,capacity,type,status,currentRouteId"
     */
    std::string toCSV() const;

    // ── Getters ───────────────────────────────────────────────────────────
    std::string getBusNumber()      const;
    int         getCapacity()       const;
    std::string getType()           const;
    std::string getStatus()         const;
    std::string getCurrentRouteId() const;

    // ── Setters ───────────────────────────────────────────────────────────
    void setBusNumber(const std::string& bn);
    void setCapacity(int cap);
    void setType(const std::string& t);
    void setStatus(const std::string& s);
    void setCurrentRouteId(const std::string& rid);
};


// =============================================================================
//  CONCRETE CLASS — Stop  (IS-A TransportEntity)
// =============================================================================
/**
 * @brief Represents a physical bus stop on the Mumbai BEST network.
 *
 * Inherits identity (id, name) from TransportEntity and adds stop-specific
 * fields:
 *   - stopCode   : Short unique code, e.g. "CBS-001"
 *   - landmark   : Nearby landmark description
 *   - latitude   : GPS latitude  (WGS-84 decimal degrees)
 *   - longitude  : GPS longitude (WGS-84 decimal degrees)
 *   - routeCount : Number of routes that pass through this stop
 *
 * Special method:
 *   - distanceTo(other) : Haversine great-circle distance in km between
 *                         this stop and another stop
 *
 * CSV serialisation format (toCSV):
 *   id,name,stopCode,landmark,latitude,longitude,routeCount
 *
 * calculateMetric() returns (double)routeCount for hub-priority charts.
 */
class Stop : public TransportEntity {
private:
    std::string stopCode;   ///< Short unique stop code, e.g. "CBS-001"
    std::string landmark;   ///< Common landmark near this stop
    double      latitude;   ///< GPS latitude  (decimal degrees, WGS-84)
    double      longitude;  ///< GPS longitude (decimal degrees, WGS-84)
    int         routeCount; ///< Number of routes serving this stop

public:
    // ── Constructors ──────────────────────────────────────────────────────
    Stop();
    Stop(int id, const std::string& name, const std::string& stopCode,
         const std::string& landmark, double latitude, double longitude,
         int routeCount);

    // ── Overridden Virtual Functions ──────────────────────────────────────
    void   getInfo()         const override; ///< Prints stop details to stdout
    double calculateMetric() const override; ///< Returns (double)routeCount

    // ── Haversine Distance Computation ────────────────────────────────────
    /**
     * @brief Computes the great-circle (Haversine) distance to another stop.
     *
     * Uses Earth's mean radius R = 6371.0 km.
     * Formula:
     *   a = sin²(Δlat/2) + cos(lat1)·cos(lat2)·sin²(Δlon/2)
     *   c = 2·atan2(√a, √(1−a))
     *   d = R · c
     *
     * @param other  The destination Stop object.
     * @return Distance in kilometres (double).
     */
    double distanceTo(const Stop& other) const;

    // ── CSV Serialisation ─────────────────────────────────────────────────
    /**
     * @brief Serialises stop data as a single comma-separated string.
     * @return "id,name,stopCode,landmark,latitude,longitude,routeCount"
     */
    std::string toCSV() const;

    // ── Getters ───────────────────────────────────────────────────────────
    std::string getStopCode()  const;
    std::string getLandmark()  const;
    double      getLatitude()  const;
    double      getLongitude() const;
    int         getRouteCount() const;

    // ── Setters ───────────────────────────────────────────────────────────
    void setStopCode(const std::string& sc);
    void setLandmark(const std::string& lm);
    void setLatitude(double lat);
    void setLongitude(double lon);
    void setRouteCount(int rc);
};


// =============================================================================
//  CONCRETE CLASS — Route  (IS-A TransportEntity)
// =============================================================================
/**
 * @brief Represents a numbered bus route on the Mumbai BEST network.
 *
 * Inherits identity (id, name) from TransportEntity and adds route-specific
 * fields:
 *   - routeNumber   : Route identifier string, e.g. "R-101"
 *   - startStop     : Name of the originating stop
 *   - endStop       : Name of the terminal stop
 *   - totalDistance : Total route length in kilometres
 *   - stopList      : Ordered list of intermediate stop names
 *
 * CSV serialisation format (toCSV):
 *   id,name,routeNumber,startStop,endStop,totalDistance,stop1;stop2;...
 *   (stops are semicolon-separated within the last CSV column)
 *
 * calculateMetric() returns totalDistance for network-coverage charts.
 */
class Route : public TransportEntity {
private:
    std::string              routeNumber;   ///< Route ID string, e.g. "R-101"
    std::string              startStop;     ///< Name of origin stop
    std::string              endStop;       ///< Name of terminal stop
    double                   totalDistance; ///< Route length in kilometres
    std::vector<std::string> stopList;      ///< Ordered sequence of stop names

public:
    // ── Constructors ──────────────────────────────────────────────────────
    Route();
    Route(int id, const std::string& name, const std::string& routeNumber,
          const std::string& startStop, const std::string& endStop,
          double totalDistance, const std::vector<std::string>& stopList);

    // ── Overridden Virtual Functions ──────────────────────────────────────
    void   getInfo()         const override; ///< Prints route details to stdout
    double calculateMetric() const override; ///< Returns totalDistance

    // ── CSV Serialisation ─────────────────────────────────────────────────
    /**
     * @brief Serialises route data as a single comma-separated string.
     *
     * The stopList is stored as a semicolon-delimited sub-field in column 7
     * so that commas inside stop names don't break CSV parsing.
     *
     * @return "id,name,routeNumber,startStop,endStop,totalDistance,s1;s2;..."
     */
    std::string toCSV() const;

    // ── Utility ───────────────────────────────────────────────────────────
    /**
     * @brief Returns the number of stops on this route (including endpoints).
     * @return Size of stopList as int.
     */
    int getStopCount() const;

    // ── Getters ───────────────────────────────────────────────────────────
    std::string              getRouteNumber()   const;
    std::string              getStartStop()     const;
    std::string              getEndStop()       const;
    double                   getTotalDistance() const;
    std::vector<std::string> getStopList()      const;

    // ── Setters ───────────────────────────────────────────────────────────
    void setRouteNumber(const std::string& rn);
    void setStartStop(const std::string& ss);
    void setEndStop(const std::string& es);
    void setTotalDistance(double td);
    void setStopList(const std::vector<std::string>& sl);
};


// =============================================================================
//  COMPOSITION CLASS — Schedule  (HAS-A Bus & Route — NOT IS-A)
// =============================================================================
/**
 * @brief Represents a time-table entry linking one Bus to one Route.
 *
 * Schedule demonstrates the COMPOSITION relationship:
 *   - Schedule HAS-A bus (referenced by busId / bus number)
 *   - Schedule HAS-A route (referenced by routeId / route number)
 *
 * It intentionally does NOT inherit from TransportEntity because a schedule
 * is not itself a transport entity — it is a timetable record that describes
 * when and on which days a bus operates a given route.
 *
 * Fields:
 *   - id            : Unique schedule ID (auto-incremented)
 *   - busId         : The bus number, e.g. "MH-01-1234"
 *   - routeId       : The route number, e.g. "R-101"
 *   - departureTime : Departure time string, e.g. "06:00"
 *   - arrivalTime   : Arrival time string,   e.g. "07:15"
 *   - days          : Day pattern, e.g. "Mon-Sat" | "Mon-Sun" | "Mon-Fri"
 *
 * CSV serialisation format (toCSV):
 *   id,busId,routeId,departureTime,arrivalTime,days
 */
class Schedule {
private:
    int         id;            ///< Auto-incremented unique schedule ID
    std::string busId;         ///< Bus number acting as foreign key to Bus
    std::string routeId;       ///< Route number acting as foreign key to Route
    std::string departureTime; ///< Departure time, e.g. "06:00"
    std::string arrivalTime;   ///< Arrival time,   e.g. "07:15"
    std::string days;          ///< Operating days, e.g. "Mon-Sat"

public:
    // ── Constructors ──────────────────────────────────────────────────────
    Schedule();
    Schedule(int id, const std::string& busId, const std::string& routeId,
             const std::string& departureTime, const std::string& arrivalTime,
             const std::string& days);

    // ── Info Display ──────────────────────────────────────────────────────
    /**
     * @brief Prints all schedule fields to stdout in a formatted box.
     * @note  Non-virtual — Schedule is not part of the polymorphic hierarchy.
     */
    void getInfo() const;

    // ── CSV Serialisation ─────────────────────────────────────────────────
    /**
     * @brief Serialises schedule as a single comma-separated string.
     * @return "id,busId,routeId,departureTime,arrivalTime,days"
     */
    std::string toCSV() const;

    // ── Getters ───────────────────────────────────────────────────────────
    int         getId()            const;
    std::string getBusId()         const;
    std::string getRouteId()       const;
    std::string getDepartureTime() const;
    std::string getArrivalTime()   const;
    std::string getDays()          const;

    // ── Setters ───────────────────────────────────────────────────────────
    void setId(int newId);
    void setBusId(const std::string& bid);
    void setRouteId(const std::string& rid);
    void setDepartureTime(const std::string& dt);
    void setArrivalTime(const std::string& at);
    void setDays(const std::string& d);
};

#endif // CLASSES_H
