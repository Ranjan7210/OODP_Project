#ifndef CLASSES_H // If CLASSES_H is not defined
#define CLASSES_H // Define CLASSES_H to prevent multiple inclusions

#include <string>  // Standard library for using text strings
#include <vector>  // Standard library for using list-like containers
#include <iostream> // Standard library for printing to screen
#include <sstream>  // Standard library for text stream processing
#include <cmath>    // Standard library for math functions

using namespace std; // Use standard namespace to avoid writing std:: everywhere

// --- THE BASE CLASS: TransportEntity ---
// This is an "Abstract" class because it has pure virtual functions (= 0)
class TransportEntity { // Start of the TransportEntity class
protected: // These variables can be accessed by child classes
    int id; // A number to uniquely identify this item
    string name; // A name to describe this item
public: // These functions can be called from anywhere
    TransportEntity(); // A default constructor (sets initial values)
    TransportEntity(int id, string name); // Constructor with specific values
    virtual ~TransportEntity(); // A virtual destructor for safe memory cleanup
    virtual void getInfo() const = 0; // A function to show details (must be implemented by children)
    virtual double calculateMetric() const = 0; // A function to get a number for reports
    int getId() const; // Function to get the ID number
    string getName() const; // Function to get the Name string
    void setId(int newId); // Function to change the ID
    void setName(string newName); // Function to change the Name
}; // End of the TransportEntity class

// --- THE BUS CLASS: Inherits from TransportEntity ---
class Bus : public TransportEntity { // Bus "is-a" TransportEntity
private: // These variables are private to protect the data (Encapsulation)
    string busNumber; // The registration number of the bus
    int capacity; // How many seats the bus has
    string type; // Whether it is "AC" or "Non-AC"
    string status; // Current state like "Active" or "Maintenance"
    string currentRouteId; // The ID of the route this bus is assigned to
public: // Publicly accessible functions
    Bus(); // Default constructor
    Bus(int id, string name, string busNum, int cap, string t, string s, string rId); // Main constructor
    void getInfo() const; // Implementation of the info display
    double calculateMetric() const; // Implementation of the metric (returns capacity)
    string toCSV() const; // Converts bus data into one comma-separated line for saving
    string getBusNumber() const; // Get the bus number
    string getStatus() const; // Get the status
    void setStatus(string s); // Set a new status
}; // End of the Bus class

// --- THE STOP CLASS: Inherits from TransportEntity ---
class Stop : public TransportEntity { // Stop "is-a" TransportEntity
private: // Data for specific bus stops
    string stopCode; // A short code like "CBS-001"
    string landmark; // A nearby place to help people find it
    double latitude; // Map coordinate (latitude)
    double longitude; // Map coordinate (longitude)
public: // Public functions
    Stop(); // Default constructor
    Stop(int id, string name, string code, string land, double lat, double lon); // Main constructor
    void getInfo() const; // Implementation of the info display
    double calculateMetric() const; // Implementation of the metric (returns 1.0)
    double distanceTo(const Stop& other) const; // A function to find distance to another stop
    string toCSV() const; // Converts stop data for CSV files
    string getStopCode() const; // Get the stop code
}; // End of the Stop class

// --- THE ROUTE CLASS: Inherits from TransportEntity ---
class Route : public TransportEntity { // Route "is-a" TransportEntity
private: // Data for bus routes
    string routeNumber; // The name of the route like "R-101"
    string startStop; // Where the route starts
    string endStop; // Where the route ends
    double totalDistance; // How long the route is in kilometers
    vector<string> stopList; // A list of all stops along this route
public: // Public functions
    Route(); // Default constructor
    Route(int id, string name, string num, string start, string end, double dist, vector<string> stops); // Constructor
    void getInfo() const; // Print route details
    double calculateMetric() const; // Return distance for reports
    string toCSV() const; // Format data for saving
    string getRouteNumber() const; // Get the route number
    vector<string> getStopList() const; // Get the list of stops
}; // End of the Route class

// --- THE SCHEDULE CLASS: A standalone class ---
class Schedule { // This class links a Bus to a Route at a specific time
private: // Schedule data
    int id; // Unique schedule ID
    string busId; // Which bus is used
    string routeId; // Which route it follows
    string departureTime; // When it leaves
    string arrivalTime; // When it arrives
    string days; // Days it runs (e.g., "Mon-Fri")
public: // Public functions
    Schedule(); // Default constructor
    Schedule(int id, string bId, string rId, string dep, string arr, string d); // Main constructor
    void getInfo() const; // Print schedule details
    string toCSV() const; // Format data for saving
    int getId() const; // Get the schedule ID
    string getBusId() const; // Get the bus number
    string getDepartureTime() const; // Get the departure time
}; // End of the Schedule class

#endif // End of the header guard
