#ifndef CLASSES_H
#define CLASSES_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>

// ============================================================
// Abstract Base Class — TransportEntity
// Demonstrates: Inheritance & Polymorphism (pure virtual)
// ============================================================
class TransportEntity {
protected:
    int id;
    std::string name;

public:
    TransportEntity() : id(0), name("") {}
    TransportEntity(int id, const std::string& name) : id(id), name(name) {}

    // Pure virtual functions — must be overridden
    virtual void getInfo() const = 0;
    virtual double calculateMetric() const = 0;

    // Virtual destructor for safe polymorphic deletion
    virtual ~TransportEntity() {}

    // Getters (Encapsulation)
    int getId() const { return id; }
    std::string getName() const { return name; }

    // Setters
    void setId(int newId) { id = newId; }
    void setName(const std::string& newName) { name = newName; }
};

// ============================================================
// Bus Class — inherits TransportEntity
// ============================================================
class Bus : public TransportEntity {
private:
    std::string busNumber;
    int capacity;
    std::string type;          // "AC" or "Non-AC"
    std::string status;        // "Active" or "Maintenance"
    std::string currentRouteId;

public:
    Bus();
    Bus(int id, const std::string& name, const std::string& busNumber, int capacity,
        const std::string& type, const std::string& status, const std::string& currentRouteId);

    // Override pure virtual functions
    void getInfo() const override;
    double calculateMetric() const override;  // Returns capacity

    // CSV serialisation
    std::string toCSV() const;

    // Getters
    std::string getBusNumber() const { return busNumber; }
    int getCapacity() const { return capacity; }
    std::string getType() const { return type; }
    std::string getStatus() const { return status; }
    std::string getCurrentRouteId() const { return currentRouteId; }

    // Setters
    void setBusNumber(const std::string& bn) { busNumber = bn; }
    void setCapacity(int cap) { capacity = cap; }
    void setType(const std::string& t) { type = t; }
    void setStatus(const std::string& s) { status = s; }
    void setCurrentRouteId(const std::string& rid) { currentRouteId = rid; }
};

// ============================================================
// Stop Class — inherits TransportEntity
// ============================================================
class Stop : public TransportEntity {
private:
    std::string stopCode;
    std::string landmark;
    double latitude;
    double longitude;
    int routeCount;

public:
    Stop();
    Stop(int id, const std::string& name, const std::string& stopCode, const std::string& landmark,
         double latitude, double longitude, int routeCount);

    // Override pure virtual functions
    void getInfo() const override;
    double calculateMetric() const override;  // Returns routeCount

    // Haversine distance to another stop (km)
    double distanceTo(const Stop& other) const;

    // CSV serialisation
    std::string toCSV() const;

    // Getters
    std::string getStopCode() const { return stopCode; }
    std::string getLandmark() const { return landmark; }
    double getLatitude() const { return latitude; }
    double getLongitude() const { return longitude; }
    int getRouteCount() const { return routeCount; }

    // Setters
    void setStopCode(const std::string& sc) { stopCode = sc; }
    void setLandmark(const std::string& lm) { landmark = lm; }
    void setLatitude(double lat) { latitude = lat; }
    void setLongitude(double lon) { longitude = lon; }
    void setRouteCount(int rc) { routeCount = rc; }
};

// ============================================================
// Route Class — inherits TransportEntity
// ============================================================
class Route : public TransportEntity {
private:
    std::string routeNumber;
    std::string startStop;
    std::string endStop;
    double totalDistance;
    std::vector<std::string> stopList;

public:
    Route();
    Route(int id, const std::string& name, const std::string& routeNumber, const std::string& startStop,
          const std::string& endStop, double totalDistance, const std::vector<std::string>& stopList);

    // Override pure virtual functions
    void getInfo() const override;
    double calculateMetric() const override;  // Returns totalDistance

    // CSV serialisation
    std::string toCSV() const;

    // Getters
    std::string getRouteNumber() const { return routeNumber; }
    std::string getStartStop() const { return startStop; }
    std::string getEndStop() const { return endStop; }
    double getTotalDistance() const { return totalDistance; }
    std::vector<std::string> getStopList() const { return stopList; }

    // Setters
    void setRouteNumber(const std::string& rn) { routeNumber = rn; }
    void setStartStop(const std::string& ss) { startStop = ss; }
    void setEndStop(const std::string& es) { endStop = es; }
    void setTotalDistance(double td) { totalDistance = td; }
    void setStopList(const std::vector<std::string>& sl) { stopList = sl; }
};

// ============================================================
// Schedule Class — Composition (HAS-A Bus & Route, not IS-A)
// ============================================================
class Schedule {
private:
    int id;
    std::string busId;
    std::string routeId;
    std::string departureTime;
    std::string arrivalTime;
    std::string days;

public:
    Schedule();
    Schedule(int id, const std::string& busId, const std::string& routeId,
             const std::string& departureTime, const std::string& arrivalTime, const std::string& days);

    // Info display
    void getInfo() const;

    // CSV serialisation
    std::string toCSV() const;

    // Getters
    int getId() const { return id; }
    std::string getBusId() const { return busId; }
    std::string getRouteId() const { return routeId; }
    std::string getDepartureTime() const { return departureTime; }
    std::string getArrivalTime() const { return arrivalTime; }
    std::string getDays() const { return days; }

    // Setters
    void setId(int newId) { id = newId; }
    void setBusId(const std::string& bid) { busId = bid; }
    void setRouteId(const std::string& rid) { routeId = rid; }
    void setDepartureTime(const std::string& dt) { departureTime = dt; }
    void setArrivalTime(const std::string& at) { arrivalTime = at; }
    void setDays(const std::string& d) { days = d; }
};

#endif // CLASSES_H
