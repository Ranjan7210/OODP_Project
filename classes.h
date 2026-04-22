#ifndef CLASSES_H 
#define CLASSES_H

#include <string>  
#include <vector>  
#include <iostream>
#include <sstream> 
#include <cmath>    

using namespace std; 

// --- THE BASE CLASS: TransportEntity ---
// "Abstract" class - pure virtual functions
class TransportEntity { 
protected: 
    int id; 
    string name; 
public: 
    TransportEntity(); // A default constructor 
    TransportEntity(int id, string name); // Constructor 
    virtual ~TransportEntity(); //destructor
    virtual void getInfo() const = 0; // A function to show details
    virtual double calculateMetric() const = 0; // A function to get a number for reports
    int getId() const; // Function to get the ID number
    string getName() const; // Function to get the Name string
    void setId(int newId); // Function to change the ID
    void setName(string newName); // Function to change the Name
};

//Inherited class: Inherits from TransportEntity
class Bus : public TransportEntity { 
private: //Encapsulation
    string busNumber; 
    int capacity; 
    string type;
    string status; 
    string currentRouteId; 
public: 
    Bus(); // Default constructor
    Bus(int id, string name, string busNum, int cap, string t, string s, string rId); //constructor
    void getInfo() const; 
    double calculateMetric() const; 
    string toCSV() const; 
    string getBusNumber() const; 
    string getStatus() const;
    void setStatus(string s);
}; 

// Inherited class: Inherit from TransportEntity
class Stop : public TransportEntity { 
private: 
    string stopCode;
    string landmark; 
    double latitude; 
    double longitude;
public: 
    Stop(); // Default constructor
    Stop(int id, string name, string code, string land, double lat, double lon); //constructor
    void getInfo() const; 
    double calculateMetric() const; 
    double distanceTo(const Stop& other) const; // A function to find distance to another stop
    string toCSV() const;
    string getStopCode() const; 
}; 

// Inherited class: Inherits from TransportEntity
class Route : public TransportEntity { y
private:
    string routeNumber; 
    string startStop; 
    string endStop; 
    double totalDistance; 
    vector<string> stopList; 
public: 
    Route(); // Default constructor
    Route(int id, string name, string num, string start, string end, double dist, vector<string> stops); // Constructor
    void getInfo() const;
    double calculateMetric() const;
    string toCSV() const; 
    string getRouteNumber() const; 
    vector<string> getStopList() const; 
}; 

//SCHEDULE CLASS
class Schedule { 
private: 
    int id; 
    string busId; 
    string routeId; 
    string departureTime;
    string arrivalTime;
    string days; 
public:
    Schedule(); // Default constructor
    Schedule(int id, string bId, string rId, string dep, string arr, string d); //constructor
    void getInfo() const; 
    string toCSV() const; 
    int getId() const; 
    string getBusId() const; 
    string getDepartureTime() const; 
};

#endif
