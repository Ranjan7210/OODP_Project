#include "classes.h" 
#include <iostream>  
#include <fstream>   
#include <vector>    
#include <string>    
#include <iomanip>   

using namespace std; 

// GLOBAL VARIABLES
vector<Bus> buses; 
vector<Stop> stops;
vector<Route> routes;
vector<Schedule> schedules;

// --- FILE SAVING FUNCTIONS ---
void saveAll() { // A function to save everything to files
    ofstream bFile("data/buses.csv"); 
    bFile << "id,name,num,cap,type,status,rid\n"; 
    for(int i=0; i < (int)buses.size(); i++) bFile << buses[i].toCSV() << endl; 
    
    ofstream sFile("data/stops.csv"); 
    sFile << "id,name,code,land,lat,lon\n"; 
    for(int i=0; i < (int)stops.size(); i++) sFile << stops[i].toCSV() << endl; 

    ofstream rFile("data/routes.csv"); 
    rFile << "id,name,num,start,end,dist,stops\n";
    for(int i=0; i < (int)routes.size(); i++) rFile << routes[i].toCSV() << endl;

    ofstream scFile("data/schedules.csv"); 
    scFile << "id,busId,routeId,dep,arr,days\n"; 
    for(int i=0; i < (int)schedules.size(); i++) scFile << schedules[i].toCSV() << endl; 
}

//LOADING FUNCTIONS 
void loadData() { // Function to load all data from CSVs
    ifstream bFile("data/buses.csv"); 
    if(bFile.is_open()) { 
        string line; getline(bFile, line); 
        while(getline(bFile, line)) { 
            stringstream ss(line); string id,n,num,cap,t,s,r; 
            getline(ss,id,','); getline(ss,n,','); getline(ss,num,','); getline(ss,cap,','); 
            getline(ss,t,','); getline(ss,s,','); getline(ss,r,','); 
            buses.push_back(Bus(stoi(id),n,num,stoi(cap),t,s,r));
        }
    }
}

// --- BUS MANAGEMENT ---
void busMenu() { 
    int choice; 
    cout << "\n1. Add Bus\n2. View Buses\n3. Back\nEnter: "; 
    cin >> choice; 
    if(choice == 1) { 
        int id = (int)buses.size() + 1; 
        string n, num, t, s, r; int c; 
        cout << "Name: "; cin >> n; cout << "Plate: "; cin >> num;
        cout << "Cap: "; cin >> c; cout << "Type (AC/Non-AC): "; cin >> t; 
        buses.push_back(Bus(id, n, num, c, t, "Active", "None")); 
        saveAll(); 
    } else if(choice == 2) { 
        for(int i=0; i < (int)buses.size(); i++) buses[i].getInfo(); 
    }
}

// --- STOP MANAGEMENT ---
void stopMenu() { 
    int choice; cout << "\n1. Add Stop\n2. View Stops\n3. Back\nEnter: "; cin >> choice; 
    if(choice == 1) { 
        string n, c, l; double lat, lon; 
        cout << "Name: "; cin >> n; cout << "Code: "; cin >> c; 
        cout << "Landmark: "; cin >> l; cout << "Lat: "; cin >> lat; cout << "Lon: "; cin >> lon; 
        stops.push_back(Stop((int)stops.size()+1, n, c, l, lat, lon)); 
        saveAll(); 
    } else if(choice == 2) { 
        for(int i=0; i < (int)stops.size(); i++) stops[i].getInfo();
    }
}

// --- REPORTS ---
void showReports() { 
    cout << "\n--- POLYMORPHIC REPORT ---" << endl; 
    vector<TransportEntity*> list; // Create a list of base class pointers
    for(int i=0; i < (int)buses.size(); i++) list.push_back(&buses[i]); 
    for(int i=0; i < (int)stops.size(); i++) list.push_back(&stops[i]); 
    
    for(int i=0; i < (int)list.size(); i++) { 
        cout << "Metric: " << list[i]->calculateMetric() << endl; // Call virtual function
        list[i]->getInfo(); // Call another virtual function
    }
}

// --- MAIN FUNCTION ---
int main() { 
    loadData(); 
    int choice = 0; 
    while(choice != 5) {
        cout << "\n=== MAIN MENU ===\n1. Bus Mgmt\n2. Stop Mgmt\n3. Reports\n4. Seed Data\n5. Exit\nChoice: "; 
        cin >> choice; 
        if(choice == 1) busMenu();
        else if(choice == 2) stopMenu(); 
        else if(choice == 3) showReports(); 
        else if(choice == 4) { 
            buses.push_back(Bus(1, "BEST-01", "MH-01", 50, "AC", "Active", "R101")); 
            stops.push_back(Stop(1, "Station", "S01", "Near Rail", 18.9, 72.8));
            saveAll();
            cout << "Data Seeded!" << endl;
        }
    }
    return 0;
}
