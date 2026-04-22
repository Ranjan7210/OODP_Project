#include "classes.h" 
#include <iomanip>   
#include <iostream>  
#include <sstream>  
#include <cmath>   

using namespace std; 

// =============================================================================
//  TRANSPORT ENTITY - Implementation
// =============================================================================

TransportEntity::TransportEntity() : id(0), name("") {} // Default constructor

TransportEntity::TransportEntity(int id, string name) : id(id), name(name) {} // Parameter constructor

TransportEntity::~TransportEntity() {} //destructor

int TransportEntity::getId() const { return id; } 

string TransportEntity::getName() const { return name; } 

void TransportEntity::setId(int newId) { id = newId; } 

void TransportEntity::setName(string newName) { name = newName; } 


// =============================================================================
//  BUS - Implementation
// =============================================================================

Bus::Bus() : TransportEntity(), busNumber(""), capacity(0), type("Non-AC"), status("Active"), currentRouteId("") {} // Default constructor

Bus::Bus(int id, string name, string busNum, int cap, string t, string s, string rId) // Parameter constructor
    : TransportEntity(id, name), busNumber(busNum), capacity(cap), type(t), status(s), currentRouteId(rId) {} 

void Bus::getInfo() const { 
    cout << "--- BUS INFO ---" << endl; 
    cout << "ID: " << id << " | Name: " << name << endl; 
    cout << "Number: " << busNumber << " | Cap: " << capacity << endl; 
    cout << "Type: " << type << " | Status: " << status << endl; 
    cout << "Route: " << currentRouteId << endl; 
}

double Bus::calculateMetric() const { return (double)capacity; } 

string Bus::toCSV() const { 
    return to_string(id) + "," + name + "," + busNumber + "," + to_string(capacity) + "," + type + "," + status + "," + currentRouteId; 
}

string Bus::getBusNumber() const { return busNumber; } 

string Bus::getStatus() const { return status; } 

void Bus::setStatus(string s) { status = s; } 


// =============================================================================
//  STOP - Implementation
// =============================================================================

Stop::Stop() : TransportEntity(), stopCode(""), landmark(""), latitude(0.0), longitude(0.0) {} // Default constructor

Stop::Stop(int id, string name, string code, string land, double lat, double lon) // Parameter constructor
    : TransportEntity(id, name), stopCode(code), landmark(land), latitude(lat), longitude(lon) {} 

void Stop::getInfo() const { 
    cout << "--- STOP INFO ---" << endl; 
    cout << "ID: " << id << " | Name: " << name << " | Code: " << stopCode << endl; 
    cout << "Landmark: " << landmark << endl; 
    cout << "Location: (" << latitude << ", " << longitude << ")" << endl; 
}

double Stop::calculateMetric() const { return 1.0; }

double Stop::distanceTo(const Stop& other) const { /
    double dLat = abs(latitude - other.latitude);
    double dLon = abs(longitude - other.longitude); 
    return (dLat + dLon) * 111.0; 
}

string Stop::toCSV() const { 
    stringstream ss; 
    ss << id << "," << name << "," << stopCode << "," << landmark << "," << latitude << "," << longitude; 
    return ss.str(); 
}

string Stop::getStopCode() const { return stopCode; } 


// =============================================================================
//  ROUTE - Implementation
// =============================================================================

Route::Route() : TransportEntity(), routeNumber(""), startStop(""), endStop(""), totalDistance(0.0) {} // Default constructor

Route::Route(int id, string name, string num, string start, string end, double dist, vector<string> stops) // Parameter constructor
    : TransportEntity(id, name), routeNumber(num), startStop(start), endStop(end), totalDistance(dist), stopList(stops) {} 

void Route::getInfo() const { 
    cout << "--- ROUTE INFO ---" << endl; 
    cout << "ID: " << id << " | Name: " << name << " | No: " << routeNumber << endl; 
    cout << "From: " << startStop << " To: " << endStop << endl; 
    cout << "Distance: " << totalDistance << " km" << endl; 
    cout << "Stops: "; 
    for(int i=0; i < (int)stopList.size(); i++) cout << stopList[i] << (i == (int)stopList.size()-1 ? "" : " -> "); 
    cout << endl; 
}

double Route::calculateMetric() const { return totalDistance; } 

string Route::toCSV() const { 
    stringstream ss; 
    ss << id << "," << name << "," << routeNumber << "," << startStop << "," << endStop << "," << totalDistance << ","; 
    for(int i=0; i < (int)stopList.size(); i++) ss << stopList[i] << (i == (int)stopList.size()-1 ? "" : ";"); 
    return ss.str(); 
}

string Route::getRouteNumber() const { return routeNumber; } 

vector<string> Route::getStopList() const { return stopList; } 


// =============================================================================
//  SCHEDULE - Implementation
// =============================================================================

Schedule::Schedule() : id(0), busId(""), routeId(""), departureTime(""), arrivalTime(""), days("") {} // Default constructor

Schedule::Schedule(int id, string bId, string rId, string dep, string arr, string d) // constructor
    : id(id), busId(bId), routeId(rId), departureTime(dep), arrivalTime(arr), days(d) {} 

void Schedule::getInfo() const { 
    cout << "--- SCHEDULE INFO ---" << endl; 
    cout << "ID: " << id << " | Bus: " << busId << " | Route: " << routeId << endl; 
    cout << "Dep: " << departureTime << " | Arr: " << arrivalTime << " | Days: " << days << endl; 
}

string Schedule::toCSV() const { 
    return to_string(id) + "," + busId + "," + routeId + "," + departureTime + "," + arrivalTime + "," + days; 
}

int Schedule::getId() const { return id; } 

string Schedule::getBusId() const { return busId; } 

string Schedule::getDepartureTime() const { return departureTime; } 
