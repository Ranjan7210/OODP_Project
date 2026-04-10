#include "classes.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <cstdlib>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif

using namespace std;

// ============================================================
// Global Vectors
// ============================================================
vector<Bus> buses;
vector<Stop> stops;
vector<Route> routes;
vector<Schedule> schedules;

const string DATA_DIR = "data/";

// ============================================================
// Utility — ensure data/ directory exists
// ============================================================
void ensureDataDir() {
    MKDIR("data");
}

// ============================================================
// LOAD FUNCTIONS — read CSV files on startup
// ============================================================

void loadBuses() {
    buses.clear();
    ifstream file(DATA_DIR + "buses.csv");
    if (!file.is_open()) return;

    string line;
    getline(file, line); // skip header
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string sid, sname, busNum, scap, stype, sstatus, routeId;
        getline(ss, sid, ',');
        getline(ss, sname, ',');
        getline(ss, busNum, ',');
        getline(ss, scap, ',');
        getline(ss, stype, ',');
        getline(ss, sstatus, ',');
        getline(ss, routeId, ',');

        try {
            int id = stoi(sid);
            int cap = stoi(scap);
            buses.push_back(Bus(id, sname, busNum, cap, stype, sstatus, routeId));
        } catch (...) {
            continue;
        }
    }
    file.close();
}

void loadStops() {
    stops.clear();
    ifstream file(DATA_DIR + "stops.csv");
    if (!file.is_open()) return;

    string line;
    getline(file, line); // skip header
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string sid, sname, scode, slandmark, slat, slon, srcount;
        getline(ss, sid, ',');
        getline(ss, sname, ',');
        getline(ss, scode, ',');
        getline(ss, slandmark, ',');
        getline(ss, slat, ',');
        getline(ss, slon, ',');
        getline(ss, srcount, ',');

        try {
            int id = stoi(sid);
            double lat = stod(slat);
            double lon = stod(slon);
            int rc = stoi(srcount);
            stops.push_back(Stop(id, sname, scode, slandmark, lat, lon, rc));
        } catch (...) {
            continue;
        }
    }
    file.close();
}

void loadRoutes() {
    routes.clear();
    ifstream file(DATA_DIR + "routes.csv");
    if (!file.is_open()) return;

    string line;
    getline(file, line); // skip header
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string sid, sname, rnum, sstart, send_s, sdist, sstops;
        getline(ss, sid, ',');
        getline(ss, sname, ',');
        getline(ss, rnum, ',');
        getline(ss, sstart, ',');
        getline(ss, send_s, ',');
        getline(ss, sdist, ',');
        getline(ss, sstops); // rest of line (semicolon-separated stops)

        try {
            int id = stoi(sid);
            double dist = stod(sdist);

            vector<string> stopList;
            stringstream stopStream(sstops);
            string singleStop;
            while (getline(stopStream, singleStop, ';')) {
                if (!singleStop.empty()) stopList.push_back(singleStop);
            }

            routes.push_back(Route(id, sname, rnum, sstart, send_s, dist, stopList));
        } catch (...) {
            continue;
        }
    }
    file.close();
}

void loadSchedules() {
    schedules.clear();
    ifstream file(DATA_DIR + "schedules.csv");
    if (!file.is_open()) return;

    string line;
    getline(file, line); // skip header
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string sid, sbusId, srouteId, sdep, sarr, sdays;
        getline(ss, sid, ',');
        getline(ss, sbusId, ',');
        getline(ss, srouteId, ',');
        getline(ss, sdep, ',');
        getline(ss, sarr, ',');
        getline(ss, sdays);

        try {
            int id = stoi(sid);
            schedules.push_back(Schedule(id, sbusId, srouteId, sdep, sarr, sdays));
        } catch (...) {
            continue;
        }
    }
    file.close();
}

// ============================================================
// SAVE FUNCTIONS — write CSV after any change
// ============================================================

void saveBuses() {
    ensureDataDir();
    ofstream file(DATA_DIR + "buses.csv");
    file << "id,name,busNumber,capacity,type,status,currentRouteId" << endl;
    for (const auto& b : buses) {
        file << b.toCSV() << endl;
    }
    file.close();
}

void saveStops() {
    ensureDataDir();
    ofstream file(DATA_DIR + "stops.csv");
    file << "id,name,stopCode,landmark,latitude,longitude,routeCount" << endl;
    for (const auto& s : stops) {
        file << s.toCSV() << endl;
    }
    file.close();
}

void saveRoutes() {
    ensureDataDir();
    ofstream file(DATA_DIR + "routes.csv");
    file << "id,name,routeNumber,startStop,endStop,totalDistance,stops" << endl;
    for (const auto& r : routes) {
        file << r.toCSV() << endl;
    }
    file.close();
}

void saveSchedules() {
    ensureDataDir();
    ofstream file(DATA_DIR + "schedules.csv");
    file << "id,busId,routeId,departureTime,arrivalTime,days" << endl;
    for (const auto& s : schedules) {
        file << s.toCSV() << endl;
    }
    file.close();
}

// ============================================================
// SEED SAMPLE DATA — realistic Mumbai BEST bus data
// ============================================================

void seedSampleData() {
    cout << "\n  [*] Seeding sample Mumbai BEST bus data...\n" << endl;

    // 4 Buses
    buses.push_back(Bus(1, "BEST Bus 1", "MH-01-1234", 45, "AC", "Active", "R-101"));
    buses.push_back(Bus(2, "BEST Bus 2", "MH-04-9821", 52, "Non-AC", "Active", "R-204"));
    buses.push_back(Bus(3, "BEST Bus 3", "MH-02-5566", 45, "AC", "Maintenance", "R-101"));
    buses.push_back(Bus(4, "BEST Bus 4", "MH-03-7712", 40, "Non-AC", "Active", "R-505"));

    // 8 Stops — real Mumbai coordinates
    stops.push_back(Stop(1, "Colaba Bus Station", "CBS-001", "Gateway of India",
                         18.921984, 72.834654, 4));
    stops.push_back(Stop(2, "Churchgate Terminal", "CGT-002", "Churchgate Station",
                         18.932533, 72.826896, 3));
    stops.push_back(Stop(3, "Marine Drive", "MRD-003", "Netaji Subhash Road",
                         18.943558, 72.823410, 2));
    stops.push_back(Stop(4, "Mumbai Central", "MBC-004", "Mumbai Central Station",
                         18.968588, 72.819337, 5));
    stops.push_back(Stop(5, "Dadar TT Circle", "DTT-005", "Dadar Station",
                         19.018375, 72.843811, 6));
    stops.push_back(Stop(6, "Bandra Reclamation", "BDR-006", "Bandra Bandstand",
                         19.048815, 72.820987, 3));
    stops.push_back(Stop(7, "Andheri Station East", "ASE-007", "Andheri Station",
                         19.119437, 72.846420, 4));
    stops.push_back(Stop(8, "Borivali Bus Depot", "BBD-008", "Borivali Station",
                         19.228273, 72.856487, 2));

    // 4 Routes — realistic Mumbai BEST routes
    routes.push_back(Route(1, "Colaba-Dadar Express", "R-101",
                           "Colaba Bus Station", "Dadar TT Circle", 12.4,
                           {"Colaba Bus Station", "Churchgate Terminal", "Marine Drive", "Mumbai Central", "Dadar TT Circle"}));
    routes.push_back(Route(2, "Churchgate-Bandra Shuttle", "R-204",
                           "Churchgate Terminal", "Bandra Reclamation", 15.8,
                           {"Churchgate Terminal", "Marine Drive", "Mumbai Central", "Dadar TT Circle", "Bandra Reclamation"}));
    routes.push_back(Route(3, "South Mumbai-Andheri Route", "R-412",
                           "Colaba Bus Station", "Andheri Station East", 24.6,
                           {"Colaba Bus Station", "Mumbai Central", "Dadar TT Circle", "Bandra Reclamation", "Andheri Station East"}));
    routes.push_back(Route(4, "Western Express Link", "R-505",
                           "Dadar TT Circle", "Borivali Bus Depot", 28.9,
                           {"Dadar TT Circle", "Bandra Reclamation", "Andheri Station East", "Borivali Bus Depot"}));

    // 4 Schedules
    schedules.push_back(Schedule(1, "MH-01-1234", "R-101", "06:00", "07:15", "Mon-Sat"));
    schedules.push_back(Schedule(2, "MH-04-9821", "R-204", "06:30", "08:00", "Mon-Sun"));
    schedules.push_back(Schedule(3, "MH-03-7712", "R-505", "07:00", "09:30", "Mon-Fri"));
    schedules.push_back(Schedule(4, "MH-01-1234", "R-412", "10:00", "12:00", "Mon-Sat"));

    // Save all
    saveBuses();
    saveStops();
    saveRoutes();
    saveSchedules();

    cout << "  [+] Seed data created successfully!" << endl;
    cout << "  [+] " << buses.size() << " buses, " << stops.size() << " stops, "
         << routes.size() << " routes, " << schedules.size() << " schedules loaded.\n" << endl;
}

// ============================================================
// BUS MANAGEMENT FUNCTIONS
// ============================================================

void addBus() {
    Bus b;
    int id = buses.empty() ? 1 : buses.back().getId() + 1;
    b.setId(id);

    string input;
    int cap;

    cout << "\n  --- Add New Bus ---" << endl;
    cout << "  Enter bus name: ";
    cin.ignore();
    getline(cin, input);
    b.setName(input);

    cout << "  Enter bus number (e.g. MH-01-1234): ";
    getline(cin, input);
    b.setBusNumber(input);

    cout << "  Enter capacity: ";
    cin >> cap;
    b.setCapacity(cap);

    cout << "  Enter type (AC/Non-AC): ";
    cin >> input;
    b.setType(input);

    cout << "  Enter status (Active/Maintenance): ";
    cin >> input;
    b.setStatus(input);

    cout << "  Enter current route ID (e.g. R-101): ";
    cin >> input;
    b.setCurrentRouteId(input);

    buses.push_back(b);
    saveBuses();
    cout << "\n  [+] Bus added successfully!\n" << endl;
}

void viewBuses() {
    if (buses.empty()) {
        cout << "\n  [!] No buses found.\n" << endl;
        return;
    }
    cout << "\n  ====== ALL BUSES (" << buses.size() << ") ======\n" << endl;
    for (const auto& b : buses) {
        b.getInfo();
        cout << endl;
    }
}

void deleteBus() {
    if (buses.empty()) {
        cout << "\n  [!] No buses to delete.\n" << endl;
        return;
    }

    string busNum;
    cout << "\n  Enter bus number to delete: ";
    cin >> busNum;

    auto it = find_if(buses.begin(), buses.end(),
                      [&busNum](const Bus& b) { return b.getBusNumber() == busNum; });

    if (it != buses.end()) {
        cout << "  [+] Deleting bus: " << it->getBusNumber() << " (" << it->getName() << ")" << endl;
        buses.erase(it);
        saveBuses();
        cout << "  [+] Bus deleted successfully!\n" << endl;
    } else {
        cout << "  [!] Bus with number '" << busNum << "' not found.\n" << endl;
    }
}

void updateBusStatus() {
    if (buses.empty()) {
        cout << "\n  [!] No buses available.\n" << endl;
        return;
    }

    string busNum;
    cout << "\n  Enter bus number to toggle status: ";
    cin >> busNum;

    auto it = find_if(buses.begin(), buses.end(),
                      [&busNum](const Bus& b) { return b.getBusNumber() == busNum; });

    if (it != buses.end()) {
        string oldStatus = it->getStatus();
        string newStatus = (oldStatus == "Active") ? "Maintenance" : "Active";
        it->setStatus(newStatus);
        saveBuses();
        cout << "  [+] Status changed: " << oldStatus << " -> " << newStatus << "\n" << endl;
    } else {
        cout << "  [!] Bus with number '" << busNum << "' not found.\n" << endl;
    }
}

void busMenu() {
    int choice;
    do {
        cout << "\n  ========== BUS MANAGEMENT ==========" << endl;
        cout << "  1. Add Bus" << endl;
        cout << "  2. View All Buses" << endl;
        cout << "  3. Delete Bus" << endl;
        cout << "  4. Toggle Bus Status (Active/Maintenance)" << endl;
        cout << "  5. Back to Main Menu" << endl;
        cout << "  Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addBus(); break;
            case 2: viewBuses(); break;
            case 3: deleteBus(); break;
            case 4: updateBusStatus(); break;
            case 5: break;
            default: cout << "  [!] Invalid choice.\n"; break;
        }
    } while (choice != 5);
}

// ============================================================
// STOP MANAGEMENT FUNCTIONS
// ============================================================

void addStop() {
    Stop s;
    int id = stops.empty() ? 1 : stops.back().getId() + 1;
    s.setId(id);

    string input;
    double lat, lon;
    int rc;

    cout << "\n  --- Add New Stop ---" << endl;
    cout << "  Enter stop name: ";
    cin.ignore();
    getline(cin, input);
    s.setName(input);

    cout << "  Enter stop code (e.g. CBS-001): ";
    getline(cin, input);
    s.setStopCode(input);

    cout << "  Enter landmark: ";
    getline(cin, input);
    s.setLandmark(input);

    cout << "  Enter latitude: ";
    cin >> lat;
    s.setLatitude(lat);

    cout << "  Enter longitude: ";
    cin >> lon;
    s.setLongitude(lon);

    cout << "  Enter route count: ";
    cin >> rc;
    s.setRouteCount(rc);

    stops.push_back(s);
    saveStops();
    cout << "\n  [+] Stop added successfully!\n" << endl;
}

void viewStops() {
    if (stops.empty()) {
        cout << "\n  [!] No stops found.\n" << endl;
        return;
    }
    cout << "\n  ====== ALL STOPS (" << stops.size() << ") ======\n" << endl;
    for (const auto& s : stops) {
        s.getInfo();
        cout << endl;
    }
}

void distanceBetweenStops() {
    if (stops.size() < 2) {
        cout << "\n  [!] Need at least 2 stops to calculate distance.\n" << endl;
        return;
    }

    string code1, code2;
    cout << "\n  Enter first stop code: ";
    cin >> code1;
    cout << "  Enter second stop code: ";
    cin >> code2;

    const Stop* s1 = nullptr;
    const Stop* s2 = nullptr;

    for (const auto& s : stops) {
        if (s.getStopCode() == code1) s1 = &s;
        if (s.getStopCode() == code2) s2 = &s;
    }

    if (s1 && s2) {
        double dist = s1->distanceTo(*s2);
        cout << fixed << setprecision(2);
        cout << "\n  Distance between " << s1->getName() << " and " << s2->getName()
             << ": " << dist << " km\n" << endl;
    } else {
        if (!s1) cout << "  [!] Stop code '" << code1 << "' not found." << endl;
        if (!s2) cout << "  [!] Stop code '" << code2 << "' not found." << endl;
        cout << endl;
    }
}

void stopMenu() {
    int choice;
    do {
        cout << "\n  ========== STOP MANAGEMENT ==========" << endl;
        cout << "  1. Add Stop" << endl;
        cout << "  2. View All Stops" << endl;
        cout << "  3. Calculate Distance Between Stops" << endl;
        cout << "  4. Back to Main Menu" << endl;
        cout << "  Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addStop(); break;
            case 2: viewStops(); break;
            case 3: distanceBetweenStops(); break;
            case 4: break;
            default: cout << "  [!] Invalid choice.\n"; break;
        }
    } while (choice != 4);
}

// ============================================================
// ROUTE MANAGEMENT FUNCTIONS
// ============================================================

void addRoute() {
    Route r;
    int id = routes.empty() ? 1 : routes.back().getId() + 1;
    r.setId(id);

    string input;
    double dist;
    int numStops;

    cout << "\n  --- Add New Route ---" << endl;
    cout << "  Enter route name: ";
    cin.ignore();
    getline(cin, input);
    r.setName(input);

    cout << "  Enter route number (e.g. R-101): ";
    getline(cin, input);
    r.setRouteNumber(input);

    cout << "  Enter start stop: ";
    getline(cin, input);
    r.setStartStop(input);

    cout << "  Enter end stop: ";
    getline(cin, input);
    r.setEndStop(input);

    cout << "  Enter total distance (km): ";
    cin >> dist;
    r.setTotalDistance(dist);

    cout << "  Enter number of stops in this route: ";
    cin >> numStops;
    cin.ignore();

    vector<string> stopList;
    for (int i = 0; i < numStops; i++) {
        cout << "    Stop " << (i + 1) << ": ";
        getline(cin, input);
        stopList.push_back(input);
    }
    r.setStopList(stopList);

    routes.push_back(r);
    saveRoutes();
    cout << "\n  [+] Route added successfully!\n" << endl;
}

void viewRoutes() {
    if (routes.empty()) {
        cout << "\n  [!] No routes found.\n" << endl;
        return;
    }
    cout << "\n  ====== ALL ROUTES (" << routes.size() << ") ======\n" << endl;
    for (const auto& r : routes) {
        r.getInfo();
        cout << endl;
    }
}

void searchRoutesByStop() {
    string stopName;
    cout << "\n  Enter stop name to search: ";
    cin.ignore();
    getline(cin, stopName);

    bool found = false;
    cout << "\n  Routes containing '" << stopName << "':" << endl;
    for (const auto& r : routes) {
        vector<string> sl = r.getStopList();
        for (const auto& s : sl) {
            if (s.find(stopName) != string::npos) {
                cout << "  -> " << r.getRouteNumber() << " : " << r.getName() << endl;
                found = true;
                break;
            }
        }
    }
    if (!found) {
        cout << "  [!] No routes found containing '" << stopName << "'." << endl;
    }
    cout << endl;
}

void routeMenu() {
    int choice;
    do {
        cout << "\n  ========== ROUTE MANAGEMENT ==========" << endl;
        cout << "  1. Add Route" << endl;
        cout << "  2. View All Routes" << endl;
        cout << "  3. Search Routes By Stop" << endl;
        cout << "  4. Back to Main Menu" << endl;
        cout << "  Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addRoute(); break;
            case 2: viewRoutes(); break;
            case 3: searchRoutesByStop(); break;
            case 4: break;
            default: cout << "  [!] Invalid choice.\n"; break;
        }
    } while (choice != 4);
}

// ============================================================
// SCHEDULE MANAGEMENT FUNCTIONS
// ============================================================

void addSchedule() {
    Schedule s;
    int id = schedules.empty() ? 1 : schedules.back().getId() + 1;
    s.setId(id);

    string busId, routeId, dep, arr, days;

    cout << "\n  --- Add New Schedule ---" << endl;
    cout << "  Enter bus ID (bus number): ";
    cin >> busId;
    s.setBusId(busId);

    cout << "  Enter route ID: ";
    cin >> routeId;
    s.setRouteId(routeId);

    cout << "  Enter departure time (HH:MM): ";
    cin >> dep;
    s.setDepartureTime(dep);

    cout << "  Enter arrival time (HH:MM): ";
    cin >> arr;
    s.setArrivalTime(arr);

    cout << "  Enter days (e.g. Mon-Sat): ";
    cin.ignore();
    getline(cin, days);
    s.setDays(days);

    // Check for scheduling conflict — same bus, same departure time
    bool conflict = false;
    for (const auto& existing : schedules) {
        if (existing.getBusId() == busId && existing.getDepartureTime() == dep) {
            conflict = true;
            break;
        }
    }

    if (conflict) {
        cout << "\n  [!] CONFLICT: Bus " << busId << " already has a schedule at " << dep << "!" << endl;
        cout << "  Schedule NOT added.\n" << endl;
    } else {
        schedules.push_back(s);
        saveSchedules();
        cout << "\n  [+] Schedule added successfully!\n" << endl;
    }
}

void viewSchedules() {
    if (schedules.empty()) {
        cout << "\n  [!] No schedules found.\n" << endl;
        return;
    }
    cout << "\n  ====== ALL SCHEDULES (" << schedules.size() << ") ======\n" << endl;
    for (const auto& s : schedules) {
        s.getInfo();
        cout << endl;
    }
}

void scheduleMenu() {
    int choice;
    do {
        cout << "\n  ========== SCHEDULE MANAGEMENT ==========" << endl;
        cout << "  1. Add Schedule" << endl;
        cout << "  2. View All Schedules" << endl;
        cout << "  3. Back to Main Menu" << endl;
        cout << "  Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addSchedule(); break;
            case 2: viewSchedules(); break;
            case 3: break;
            default: cout << "  [!] Invalid choice.\n"; break;
        }
    } while (choice != 3);
}

// ============================================================
// REPORTS & OOP DEMO — Runtime Polymorphism
// ============================================================

void showReports() {
    cout << "\n  ============================================================" << endl;
    cout << "     REPORTS & OOP DEMONSTRATION — Runtime Polymorphism" << endl;
    cout << "  ============================================================\n" << endl;

    // Create a vector of base class pointers — demonstrates polymorphism
    vector<TransportEntity*> entities;

    // Push pointers to all buses
    for (auto& b : buses) entities.push_back(&b);
    // Push pointers to all stops
    for (auto& s : stops) entities.push_back(&s);
    // Push pointers to all routes
    for (auto& r : routes) entities.push_back(&r);

    cout << "  Total entities in polymorphic container: " << entities.size() << endl;
    cout << "  (Buses: " << buses.size() << ", Stops: " << stops.size()
         << ", Routes: " << routes.size() << ")\n" << endl;

    cout << "  --- Calling getInfo() and calculateMetric() via base pointer ---\n" << endl;

    for (size_t i = 0; i < entities.size(); i++) {
        cout << "  [Entity " << (i + 1) << "] Metric Value: "
             << fixed << setprecision(2) << entities[i]->calculateMetric() << endl;
        entities[i]->getInfo();
        cout << endl;
    }

    // Summary statistics
    cout << "\n  ============================================================" << endl;
    cout << "     SUMMARY STATISTICS" << endl;
    cout << "  ============================================================" << endl;

    double totalCapacity = 0, totalDistance = 0;
    int acCount = 0, nonAcCount = 0, activeCount = 0, maintCount = 0;

    for (const auto& b : buses) {
        totalCapacity += b.getCapacity();
        if (b.getType() == "AC") acCount++;
        else nonAcCount++;
        if (b.getStatus() == "Active") activeCount++;
        else maintCount++;
    }

    for (const auto& r : routes) {
        totalDistance += r.getTotalDistance();
    }

    cout << "  Total Fleet Capacity : " << totalCapacity << " seats" << endl;
    cout << "  AC Buses             : " << acCount << endl;
    cout << "  Non-AC Buses         : " << nonAcCount << endl;
    cout << "  Active Buses         : " << activeCount << endl;
    cout << "  Under Maintenance    : " << maintCount << endl;
    cout << "  Total Network Dist.  : " << fixed << setprecision(2) << totalDistance << " km" << endl;
    cout << "  Total Stops          : " << stops.size() << endl;
    cout << "  Total Schedules      : " << schedules.size() << endl;
    cout << "  ============================================================\n" << endl;

    // OOP Concepts Demonstrated
    cout << "  ============================================================" << endl;
    cout << "     OOP CONCEPTS DEMONSTRATED IN THIS PROJECT" << endl;
    cout << "  ============================================================" << endl;
    cout << "  1. INHERITANCE    : Bus, Stop, Route extend TransportEntity" << endl;
    cout << "  2. POLYMORPHISM   : vector<TransportEntity*> with virtual getInfo()" << endl;
    cout << "  3. ENCAPSULATION  : Private/protected fields with getters/setters" << endl;
    cout << "  4. COMPOSITION    : Schedule HAS-A Bus and Route (not IS-A)" << endl;
    cout << "  5. ABSTRACTION    : TransportEntity has pure virtual functions" << endl;
    cout << "  ============================================================\n" << endl;
}

// ============================================================
// MAIN FUNCTION
// ============================================================

int main() {
    ensureDataDir();

    // Load all data from CSV files
    loadBuses();
    loadStops();
    loadRoutes();
    loadSchedules();

    // Seed sample data if everything is empty (first run)
    if (buses.empty() && stops.empty() && routes.empty() && schedules.empty()) {
        seedSampleData();
    } else {
        cout << "\n  [*] Data loaded: " << buses.size() << " buses, " << stops.size()
             << " stops, " << routes.size() << " routes, " << schedules.size()
             << " schedules.\n" << endl;
    }

    int choice;
    do {
        cout << "  ============================================================" << endl;
        cout << "     BUS ROUTE MANAGEMENT SYSTEM — Mumbai BEST" << endl;
        cout << "  ============================================================" << endl;
        cout << "  1. Bus Management" << endl;
        cout << "  2. Stop Management" << endl;
        cout << "  3. Route Management" << endl;
        cout << "  4. Schedule Management" << endl;
        cout << "  5. Reports & OOP Demo" << endl;
        cout << "  6. Exit" << endl;
        cout << "  ============================================================" << endl;
        cout << "  Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: busMenu(); break;
            case 2: stopMenu(); break;
            case 3: routeMenu(); break;
            case 4: scheduleMenu(); break;
            case 5: showReports(); break;
            case 6:
                cout << "\n  [*] Exiting Bus Route Management System. Goodbye!\n" << endl;
                break;
            default:
                cout << "  [!] Invalid choice. Please try again.\n" << endl;
                break;
        }
    } while (choice != 6);

    return 0;
}
