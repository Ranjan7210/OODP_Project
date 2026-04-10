#include "classes.h"
#include <iomanip>

using namespace std;
#include <iostream>

// ============================================================
// Bus Implementation
// ============================================================

Bus::Bus() : TransportEntity(), busNumber(""), capacity(0), type("Non-AC"),
             status("Active"), currentRouteId("") {}

Bus::Bus(int id, const string& name, const string& busNumber, int capacity,
         const string& type, const string& status, const string& currentRouteId)
    : TransportEntity(id, name), busNumber(busNumber), capacity(capacity),
      type(type), status(status), currentRouteId(currentRouteId) {}

void Bus::getInfo() const {
    cout << "========================================" << endl;
    cout << "  BUS INFORMATION" << endl;
    cout << "========================================" << endl;
    cout << "  ID          : " << id << endl;
    cout << "  Name        : " << name << endl;
    cout << "  Bus Number  : " << busNumber << endl;
    cout << "  Capacity    : " << capacity << " seats" << endl;
    cout << "  Type        : " << type << endl;
    cout << "  Status      : " << status << endl;
    cout << "  Route ID    : " << currentRouteId << endl;
    cout << "========================================" << endl;
}

double Bus::calculateMetric() const {
    return static_cast<double>(capacity);
}

string Bus::toCSV() const {
    return to_string(id) + "," + name + "," + busNumber + "," +
           to_string(capacity) + "," + type + "," + status + "," + currentRouteId;
}

// ============================================================
// Stop Implementation
// ============================================================

Stop::Stop() : TransportEntity(), stopCode(""), landmark(""),
               latitude(0.0), longitude(0.0), routeCount(0) {}

Stop::Stop(int id, const string& name, const string& stopCode, const string& landmark,
           double latitude, double longitude, int routeCount)
    : TransportEntity(id, name), stopCode(stopCode), landmark(landmark),
      latitude(latitude), longitude(longitude), routeCount(routeCount) {}

void Stop::getInfo() const {
    cout << "========================================" << endl;
    cout << "  STOP INFORMATION" << endl;
    cout << "========================================" << endl;
    cout << "  ID          : " << id << endl;
    cout << "  Name        : " << name << endl;
    cout << "  Stop Code   : " << stopCode << endl;
    cout << "  Landmark    : " << landmark << endl;
    cout << fixed << setprecision(6);
    cout << "  Latitude    : " << latitude << endl;
    cout << "  Longitude   : " << longitude << endl;
    cout << "  Route Count : " << routeCount << endl;
    cout << "========================================" << endl;
}

double Stop::calculateMetric() const {
    return static_cast<double>(routeCount);
}

// Haversine formula — calculates real GPS distance between two stops in km
double Stop::distanceTo(const Stop& other) const {
    const double R = 6371.0; // Earth's radius in km
    const double PI = 3.14159265358979323846;

    double lat1 = latitude * PI / 180.0;
    double lat2 = other.latitude * PI / 180.0;
    double dlat = (other.latitude - latitude) * PI / 180.0;
    double dlon = (other.longitude - longitude) * PI / 180.0;

    double a = sin(dlat / 2.0) * sin(dlat / 2.0) +
               cos(lat1) * cos(lat2) *
               sin(dlon / 2.0) * sin(dlon / 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

    return R * c;
}

string Stop::toCSV() const {
    ostringstream oss;
    oss << id << "," << name << "," << stopCode << "," << landmark << ","
        << fixed << setprecision(6) << latitude << ","
        << fixed << setprecision(6) << longitude << "," << routeCount;
    return oss.str();
}

// ============================================================
// Route Implementation
// ============================================================

Route::Route() : TransportEntity(), routeNumber(""), startStop(""),
                 endStop(""), totalDistance(0.0) {}

Route::Route(int id, const string& name, const string& routeNumber, const string& startStop,
             const string& endStop, double totalDistance, const vector<string>& stopList)
    : TransportEntity(id, name), routeNumber(routeNumber), startStop(startStop),
      endStop(endStop), totalDistance(totalDistance), stopList(stopList) {}

void Route::getInfo() const {
    cout << "========================================" << endl;
    cout << "  ROUTE INFORMATION" << endl;
    cout << "========================================" << endl;
    cout << "  ID             : " << id << endl;
    cout << "  Name           : " << name << endl;
    cout << "  Route Number   : " << routeNumber << endl;
    cout << "  Start Stop     : " << startStop << endl;
    cout << "  End Stop       : " << endStop << endl;
    cout << fixed << setprecision(2);
    cout << "  Total Distance : " << totalDistance << " km" << endl;
    cout << "  Stops          : ";
    for (size_t i = 0; i < stopList.size(); i++) {
        cout << stopList[i];
        if (i < stopList.size() - 1) cout << " -> ";
    }
    cout << endl;
    cout << "========================================" << endl;
}

double Route::calculateMetric() const {
    return totalDistance;
}

string Route::toCSV() const {
    ostringstream oss;
    oss << id << "," << name << "," << routeNumber << "," << startStop << ","
        << endStop << "," << fixed << setprecision(2) << totalDistance << ",";
    // Store stop list as semicolon-separated within the CSV field
    for (size_t i = 0; i < stopList.size(); i++) {
        oss << stopList[i];
        if (i < stopList.size() - 1) oss << ";";
    }
    return oss.str();
}

// ============================================================
// Schedule Implementation (Composition — HAS-A Bus & Route)
// ============================================================

Schedule::Schedule() : id(0), busId(""), routeId(""),
                       departureTime(""), arrivalTime(""), days("") {}

Schedule::Schedule(int id, const string& busId, const string& routeId,
                   const string& departureTime, const string& arrivalTime, const string& days)
    : id(id), busId(busId), routeId(routeId),
      departureTime(departureTime), arrivalTime(arrivalTime), days(days) {}

void Schedule::getInfo() const {
    cout << "========================================" << endl;
    cout << "  SCHEDULE INFORMATION" << endl;
    cout << "========================================" << endl;
    cout << "  ID            : " << id << endl;
    cout << "  Bus ID        : " << busId << endl;
    cout << "  Route ID      : " << routeId << endl;
    cout << "  Departure     : " << departureTime << endl;
    cout << "  Arrival       : " << arrivalTime << endl;
    cout << "  Days          : " << days << endl;
    cout << "========================================" << endl;
}

string Schedule::toCSV() const {
    return to_string(id) + "," + busId + "," + routeId + "," +
           departureTime + "," + arrivalTime + "," + days;
}
