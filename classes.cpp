#include "classes.h" // Include header to see class definitions
#include <iomanip>   // Standard library for formatting output (like precision)
#include <iostream>  // Standard library for printing to screen
#include <sstream>   // Standard library for building strings
#include <cmath>     // Standard library for math functions

using namespace std; // Use standard namespace for shortcut

// =============================================================================
//  TRANSPORT ENTITY - Implementation
// =============================================================================

TransportEntity::TransportEntity() : id(0), name("") {} // Default constructor: sets ID to 0 and Name to empty

TransportEntity::TransportEntity(int id, string name) : id(id), name(name) {} // Parameter constructor: sets values

TransportEntity::~TransportEntity() {} // Virtual destructor: does nothing but is needed for safe inheritance

int TransportEntity::getId() const { return id; } // Returns the object's ID

string TransportEntity::getName() const { return name; } // Returns the object's Name

void TransportEntity::setId(int newId) { id = newId; } // Changes the object's ID

void TransportEntity::setName(string newName) { name = newName; } // Changes the object's Name


// =============================================================================
//  BUS - Implementation
// =============================================================================

Bus::Bus() : TransportEntity(), busNumber(""), capacity(0), type("Non-AC"), status("Active"), currentRouteId("") {} // Default constructor with defaults

Bus::Bus(int id, string name, string busNum, int cap, string t, string s, string rId) // Parameter constructor
    : TransportEntity(id, name), busNumber(busNum), capacity(cap), type(t), status(s), currentRouteId(rId) {} // Call base and set fields

void Bus::getInfo() const { // Shows bus data on screen
    cout << "--- BUS INFO ---" << endl; // Title
    cout << "ID: " << id << " | Name: " << name << endl; // ID and Name
    cout << "Number: " << busNumber << " | Cap: " << capacity << endl; // Plate and Capacity
    cout << "Type: " << type << " | Status: " << status << endl; // AC/Non-AC and Status
    cout << "Route: " << currentRouteId << endl; // Route field
}

double Bus::calculateMetric() const { return (double)capacity; } // KPI for bus is its seating capacity

string Bus::toCSV() const { // Formats data for a CSV file
    return to_string(id) + "," + name + "," + busNumber + "," + to_string(capacity) + "," + type + "," + status + "," + currentRouteId; // Comma-separated list
}

string Bus::getBusNumber() const { return busNumber; } // Returns plate number

string Bus::getStatus() const { return status; } // Returns current status

void Bus::setStatus(string s) { status = s; } // Changes status


// =============================================================================
//  STOP - Implementation
// =============================================================================

Stop::Stop() : TransportEntity(), stopCode(""), landmark(""), latitude(0.0), longitude(0.0) {} // Default constructor

Stop::Stop(int id, string name, string code, string land, double lat, double lon) // Parameter constructor
    : TransportEntity(id, name), stopCode(code), landmark(land), latitude(lat), longitude(lon) {} // Set fields

void Stop::getInfo() const { // Shows stop data
    cout << "--- STOP INFO ---" << endl; // Title
    cout << "ID: " << id << " | Name: " << name << " | Code: " << stopCode << endl; // Major fields
    cout << "Landmark: " << landmark << endl; // Nearby place
    cout << "Location: (" << latitude << ", " << longitude << ")" << endl; // Coordinates
}

double Stop::calculateMetric() const { return 1.0; } // Default metric for a stop is just 1 unit

double Stop::distanceTo(const Stop& other) const { // Simplified distance formula for beginners
    double dLat = abs(latitude - other.latitude); // Difference in latitude
    double dLon = abs(longitude - other.longitude); // Difference in longitude
    return (dLat + dLon) * 111.0; // Very rough km conversion (1 degree approx 111km)
}

string Stop::toCSV() const { // Formats for CSV file
    stringstream ss; // Use stringstream to handle decimal numbers
    ss << id << "," << name << "," << stopCode << "," << landmark << "," << latitude << "," << longitude; // Build the string
    return ss.str(); // Return completed string
}

string Stop::getStopCode() const { return stopCode; } // Returns stop code


// =============================================================================
//  ROUTE - Implementation
// =============================================================================

Route::Route() : TransportEntity(), routeNumber(""), startStop(""), endStop(""), totalDistance(0.0) {} // Default constructor

Route::Route(int id, string name, string num, string start, string end, double dist, vector<string> stops) // Parameter constructor
    : TransportEntity(id, name), routeNumber(num), startStop(start), endStop(end), totalDistance(dist), stopList(stops) {} // Build the route

void Route::getInfo() const { // Shows route details
    cout << "--- ROUTE INFO ---" << endl; // Title
    cout << "ID: " << id << " | Name: " << name << " | No: " << routeNumber << endl; // Major fields
    cout << "From: " << startStop << " To: " << endStop << endl; // Start/End
    cout << "Distance: " << totalDistance << " km" << endl; // Length
    cout << "Stops: "; // Header for stop list
    for(int i=0; i < (int)stopList.size(); i++) cout << stopList[i] << (i == (int)stopList.size()-1 ? "" : " -> "); // Print list with arrows
    cout << endl; // New line
}

double Route::calculateMetric() const { return totalDistance; } // KPI for route is its length

string Route::toCSV() const { // Formats for CSV
    stringstream ss; // Use stream for decimals and complex lists
    ss << id << "," << name << "," << routeNumber << "," << startStop << "," << endStop << "," << totalDistance << ","; // Basic fields
    for(int i=0; i < (int)stopList.size(); i++) ss << stopList[i] << (i == (int)stopList.size()-1 ? "" : ";"); // Semicolon-separated stops
    return ss.str(); // Return string
}

string Route::getRouteNumber() const { return routeNumber; } // Get route No

vector<string> Route::getStopList() const { return stopList; } // Get the stops


// =============================================================================
//  SCHEDULE - Implementation
// =============================================================================

Schedule::Schedule() : id(0), busId(""), routeId(""), departureTime(""), arrivalTime(""), days("") {} // Default constructor

Schedule::Schedule(int id, string bId, string rId, string dep, string arr, string d) // Main constructor
    : id(id), busId(bId), routeId(rId), departureTime(dep), arrivalTime(arr), days(d) {} // Assign values

void Schedule::getInfo() const { // Shows schedule data
    cout << "--- SCHEDULE INFO ---" << endl; // Title
    cout << "ID: " << id << " | Bus: " << busId << " | Route: " << routeId << endl; // IDs
    cout << "Dep: " << departureTime << " | Arr: " << arrivalTime << " | Days: " << days << endl; // Times
}

string Schedule::toCSV() const { // Formats for CSV
    return to_string(id) + "," + busId + "," + routeId + "," + departureTime + "," + arrivalTime + "," + days; // CSV Line
}

int Schedule::getId() const { return id; } // Get ID

string Schedule::getBusId() const { return busId; } // Get Bus Plate

string Schedule::getDepartureTime() const { return departureTime; } // Get Dep time
