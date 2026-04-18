/**
 * =============================================================================
 * FILE        : classes.cpp
 * PROJECT     : Bus Route Management System — Mumbai BEST
 * AUTHOR      : Member 1 (OOP Architect)
 * DESCRIPTION : Implementations of all methods declared in classes.h.
 *               Contains constructors, virtual overrides, getters, setters,
 *               CSV serialisation, and the Haversine distance formula.
 * =============================================================================
 */

#include "classes.h"
#include <iomanip>
#include <iostream>

using namespace std;


// =============================================================================
//  TransportEntity — Abstract Base Class Implementation
// =============================================================================

/**
 * Default constructor.
 * Initialises id = 0 and name = "" — safe default state before setters
 * are called or data is loaded from CSV.
 */
TransportEntity::TransportEntity()
    : id(0), name("") {}

/**
 * Parameterised constructor.
 * @param id    Unique numeric identifier.
 * @param name  Human-readable display name.
 */
TransportEntity::TransportEntity(int id, const string& name)
    : id(id), name(name) {}

/**
 * Virtual destructor — defined out-of-line to ensure the vtable is anchored
 * in this translation unit and to allow safe polymorphic deletion via a
 * TransportEntity* pointer.
 */
TransportEntity::~TransportEntity() {}

// ── Getters ──────────────────────────────────────────────────────────────────
int    TransportEntity::getId()   const { return id;   }
string TransportEntity::getName() const { return name; }

// ── Setters ──────────────────────────────────────────────────────────────────
void TransportEntity::setId(int newId)             { id   = newId;   }
void TransportEntity::setName(const string& newName) { name = newName; }


// =============================================================================
//  Bus — Concrete Derived Class Implementation
// =============================================================================

/**
 * Default constructor.
 * Delegates to TransportEntity() and sets sensible defaults:
 * capacity = 0, type = "Non-AC", status = "Active".
 */
Bus::Bus()
    : TransportEntity(),
      busNumber(""),
      capacity(0),
      type("Non-AC"),
      status("Active"),
      currentRouteId("") {}

/**
 * Parameterised constructor — used by seedSampleData() and CSV loader.
 */
Bus::Bus(int id, const string& name, const string& busNumber, int capacity,
         const string& type, const string& status, const string& currentRouteId)
    : TransportEntity(id, name),
      busNumber(busNumber),
      capacity(capacity),
      type(type),
      status(status),
      currentRouteId(currentRouteId) {}

// ── Override: getInfo() ───────────────────────────────────────────────────────
/**
 * Prints a formatted information block for this bus to stdout.
 * Called via a TransportEntity* pointer in the Reports module to demonstrate
 * runtime polymorphism.
 */
void Bus::getInfo() const {
    cout << "========================================" << endl;
    cout << "  BUS INFORMATION"                        << endl;
    cout << "========================================" << endl;
    cout << "  ID          : " << id            << endl;
    cout << "  Name        : " << name          << endl;
    cout << "  Bus Number  : " << busNumber     << endl;
    cout << "  Capacity    : " << capacity      << " seats" << endl;
    cout << "  Type        : " << type          << endl;
    cout << "  Status      : " << status        << endl;
    cout << "  Route ID    : " << currentRouteId << endl;
    cout << "========================================" << endl;
}

// ── Override: calculateMetric() ──────────────────────────────────────────────
/**
 * Returns the bus seat capacity as a double.
 * Used in fleet-capacity bar charts (Member 4) and the Reports screen.
 */
double Bus::calculateMetric() const {
    return static_cast<double>(capacity);
}

// ── CSV Serialisation ─────────────────────────────────────────────────────────
/**
 * Serialises all Bus fields as a single comma-separated line.
 * Matches the header: id,name,busNumber,capacity,type,status,currentRouteId
 */
string Bus::toCSV() const {
    return to_string(id) + "," +
           name          + "," +
           busNumber     + "," +
           to_string(capacity) + "," +
           type          + "," +
           status        + "," +
           currentRouteId;
}

// ── Getters ──────────────────────────────────────────────────────────────────
string Bus::getBusNumber()      const { return busNumber;      }
int    Bus::getCapacity()       const { return capacity;       }
string Bus::getType()           const { return type;           }
string Bus::getStatus()         const { return status;         }
string Bus::getCurrentRouteId() const { return currentRouteId; }

// ── Setters ──────────────────────────────────────────────────────────────────
void Bus::setBusNumber(const string& bn)      { busNumber      = bn;  }
void Bus::setCapacity(int cap)                { capacity       = cap; }
void Bus::setType(const string& t)            { type           = t;   }
void Bus::setStatus(const string& s)          { status         = s;   }
void Bus::setCurrentRouteId(const string& rid){ currentRouteId = rid; }


// =============================================================================
//  Stop — Concrete Derived Class Implementation
// =============================================================================

/**
 * Default constructor.
 * Delegates to TransportEntity() and initialises numeric fields to 0.
 */
Stop::Stop()
    : TransportEntity(),
      stopCode(""),
      landmark(""),
      latitude(0.0),
      longitude(0.0),
      routeCount(0) {}

/**
 * Parameterised constructor — used by seedSampleData() and CSV loader.
 */
Stop::Stop(int id, const string& name, const string& stopCode,
           const string& landmark, double latitude, double longitude,
           int routeCount)
    : TransportEntity(id, name),
      stopCode(stopCode),
      landmark(landmark),
      latitude(latitude),
      longitude(longitude),
      routeCount(routeCount) {}

// ── Override: getInfo() ───────────────────────────────────────────────────────
/**
 * Prints a formatted information block for this stop to stdout.
 * Latitude and longitude are shown with 6 decimal places for GPS precision.
 */
void Stop::getInfo() const {
    cout << "========================================" << endl;
    cout << "  STOP INFORMATION"                       << endl;
    cout << "========================================" << endl;
    cout << "  ID          : " << id         << endl;
    cout << "  Name        : " << name       << endl;
    cout << "  Stop Code   : " << stopCode   << endl;
    cout << "  Landmark    : " << landmark   << endl;
    cout << fixed << setprecision(6);
    cout << "  Latitude    : " << latitude   << endl;
    cout << "  Longitude   : " << longitude  << endl;
    cout << "  Route Count : " << routeCount << endl;
    cout << "========================================" << endl;
}

// ── Override: calculateMetric() ──────────────────────────────────────────────
/**
 * Returns the number of routes serving this stop as a double.
 * Used in hub-priority bar charts (Member 4) and the Reports screen.
 */
double Stop::calculateMetric() const {
    return static_cast<double>(routeCount);
}

// ── Haversine Distance ────────────────────────────────────────────────────────
/**
 * Computes the great-circle distance (in km) between this stop and `other`
 * using the Haversine formula.
 *
 * Haversine Formula:
 *   a = sin²((lat2-lat1)/2) + cos(lat1)·cos(lat2)·sin²((lon2-lon1)/2)
 *   c = 2·atan2(√a, √(1−a))
 *   d = R · c   where R = 6371.0 km
 *
 * @param other  The destination Stop.
 * @return Distance in kilometres.
 */
double Stop::distanceTo(const Stop& other) const {
    const double R  = 6371.0;                        // Earth's mean radius, km
    const double PI = 3.14159265358979323846;

    double lat1  = latitude        * PI / 180.0;
    double lat2  = other.latitude  * PI / 180.0;
    double dLat  = (other.latitude  - latitude)  * PI / 180.0;
    double dLon  = (other.longitude - longitude) * PI / 180.0;

    double a = sin(dLat / 2.0) * sin(dLat / 2.0) +
               cos(lat1) * cos(lat2) *
               sin(dLon / 2.0) * sin(dLon / 2.0);

    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

    return R * c;
}

// ── CSV Serialisation ─────────────────────────────────────────────────────────
/**
 * Serialises all Stop fields as a single comma-separated line.
 * lat/lon are written with 6 decimal places to preserve GPS accuracy.
 * Matches header: id,name,stopCode,landmark,latitude,longitude,routeCount
 */
string Stop::toCSV() const {
    ostringstream oss;
    oss << id        << ","
        << name      << ","
        << stopCode  << ","
        << landmark  << ","
        << fixed << setprecision(6) << latitude  << ","
        << fixed << setprecision(6) << longitude << ","
        << routeCount;
    return oss.str();
}

// ── Getters ──────────────────────────────────────────────────────────────────
string Stop::getStopCode()   const { return stopCode;   }
string Stop::getLandmark()   const { return landmark;   }
double Stop::getLatitude()   const { return latitude;   }
double Stop::getLongitude()  const { return longitude;  }
int    Stop::getRouteCount() const { return routeCount; }

// ── Setters ──────────────────────────────────────────────────────────────────
void Stop::setStopCode(const string& sc)  { stopCode   = sc;  }
void Stop::setLandmark(const string& lm)  { landmark   = lm;  }
void Stop::setLatitude(double lat)        { latitude   = lat; }
void Stop::setLongitude(double lon)       { longitude  = lon; }
void Stop::setRouteCount(int rc)          { routeCount = rc;  }


// =============================================================================
//  Route — Concrete Derived Class Implementation
// =============================================================================

/**
 * Default constructor.
 * Delegates to TransportEntity(); totalDistance = 0 and stopList is empty.
 */
Route::Route()
    : TransportEntity(),
      routeNumber(""),
      startStop(""),
      endStop(""),
      totalDistance(0.0) {}

/**
 * Parameterised constructor — used by seedSampleData() and CSV loader.
 */
Route::Route(int id, const string& name, const string& routeNumber,
             const string& startStop, const string& endStop,
             double totalDistance, const vector<string>& stopList)
    : TransportEntity(id, name),
      routeNumber(routeNumber),
      startStop(startStop),
      endStop(endStop),
      totalDistance(totalDistance),
      stopList(stopList) {}

// ── Override: getInfo() ───────────────────────────────────────────────────────
/**
 * Prints a formatted information block for this route to stdout.
 * The stop list is rendered as:  Stop1 -> Stop2 -> Stop3 ...
 */
void Route::getInfo() const {
    cout << "========================================" << endl;
    cout << "  ROUTE INFORMATION"                      << endl;
    cout << "========================================" << endl;
    cout << "  ID             : " << id            << endl;
    cout << "  Name           : " << name          << endl;
    cout << "  Route Number   : " << routeNumber   << endl;
    cout << "  Start Stop     : " << startStop     << endl;
    cout << "  End Stop       : " << endStop       << endl;
    cout << fixed << setprecision(2);
    cout << "  Total Distance : " << totalDistance << " km" << endl;
    cout << "  No. of Stops   : " << stopList.size() << endl;
    cout << "  Stops          : ";
    for (size_t i = 0; i < stopList.size(); ++i) {
        cout << stopList[i];
        if (i < stopList.size() - 1) cout << " -> ";
    }
    cout << endl;
    cout << "========================================" << endl;
}

// ── Override: calculateMetric() ──────────────────────────────────────────────
/**
 * Returns the total route distance in kilometres.
 * Used in network-coverage bar charts (Member 4) and the Reports screen.
 */
double Route::calculateMetric() const {
    return totalDistance;
}

// ── Utility: getStopCount() ───────────────────────────────────────────────────
/**
 * Returns the number of stops on this route as an int.
 * Convenience wrapper around stopList.size().
 */
int Route::getStopCount() const {
    return static_cast<int>(stopList.size());
}

// ── CSV Serialisation ─────────────────────────────────────────────────────────
/**
 * Serialises all Route fields as a single comma-separated line.
 * The stopList is stored as semicolon-separated values within column 7,
 * avoiding conflicts with the CSV comma delimiter.
 *
 * Matches header: id,name,routeNumber,startStop,endStop,totalDistance,stops
 */
string Route::toCSV() const {
    ostringstream oss;
    oss << id            << ","
        << name          << ","
        << routeNumber   << ","
        << startStop     << ","
        << endStop       << ","
        << fixed << setprecision(2) << totalDistance << ",";
    for (size_t i = 0; i < stopList.size(); ++i) {
        oss << stopList[i];
        if (i < stopList.size() - 1) oss << ";";
    }
    return oss.str();
}

// ── Getters ──────────────────────────────────────────────────────────────────
string         Route::getRouteNumber()   const { return routeNumber;   }
string         Route::getStartStop()     const { return startStop;     }
string         Route::getEndStop()       const { return endStop;       }
double         Route::getTotalDistance() const { return totalDistance; }
vector<string> Route::getStopList()      const { return stopList;      }

// ── Setters ──────────────────────────────────────────────────────────────────
void Route::setRouteNumber(const string& rn)        { routeNumber   = rn;  }
void Route::setStartStop(const string& ss)          { startStop     = ss;  }
void Route::setEndStop(const string& es)            { endStop       = es;  }
void Route::setTotalDistance(double td)             { totalDistance = td;  }
void Route::setStopList(const vector<string>& sl)   { stopList      = sl;  }


// =============================================================================
//  Schedule — Composition Class Implementation  (HAS-A Bus & Route)
// =============================================================================

/**
 * Default constructor.
 * id = 0, all strings = "".
 */
Schedule::Schedule()
    : id(0),
      busId(""),
      routeId(""),
      departureTime(""),
      arrivalTime(""),
      days("") {}

/**
 * Parameterised constructor — used by seedSampleData() and CSV loader.
 */
Schedule::Schedule(int id, const string& busId, const string& routeId,
                   const string& departureTime, const string& arrivalTime,
                   const string& days)
    : id(id),
      busId(busId),
      routeId(routeId),
      departureTime(departureTime),
      arrivalTime(arrivalTime),
      days(days) {}

// ── Info Display ──────────────────────────────────────────────────────────────
/**
 * Prints a formatted information block for this schedule to stdout.
 * Non-virtual — Schedule is outside the TransportEntity polymorphic hierarchy.
 */
void Schedule::getInfo() const {
    cout << "========================================" << endl;
    cout << "  SCHEDULE INFORMATION"                   << endl;
    cout << "========================================" << endl;
    cout << "  ID            : " << id            << endl;
    cout << "  Bus ID        : " << busId         << endl;
    cout << "  Route ID      : " << routeId       << endl;
    cout << "  Departure     : " << departureTime << endl;
    cout << "  Arrival       : " << arrivalTime   << endl;
    cout << "  Days          : " << days          << endl;
    cout << "========================================" << endl;
}

// ── CSV Serialisation ─────────────────────────────────────────────────────────
/**
 * Serialises all Schedule fields as a single comma-separated line.
 * Matches header: id,busId,routeId,departureTime,arrivalTime,days
 */
string Schedule::toCSV() const {
    return to_string(id) + "," +
           busId         + "," +
           routeId       + "," +
           departureTime + "," +
           arrivalTime   + "," +
           days;
}

// ── Getters ──────────────────────────────────────────────────────────────────
int    Schedule::getId()            const { return id;            }
string Schedule::getBusId()         const { return busId;         }
string Schedule::getRouteId()       const { return routeId;       }
string Schedule::getDepartureTime() const { return departureTime; }
string Schedule::getArrivalTime()   const { return arrivalTime;   }
string Schedule::getDays()          const { return days;          }

// ── Setters ──────────────────────────────────────────────────────────────────
void Schedule::setId(int newId)                   { id            = newId; }
void Schedule::setBusId(const string& bid)        { busId         = bid;   }
void Schedule::setRouteId(const string& rid)      { routeId       = rid;   }
void Schedule::setDepartureTime(const string& dt) { departureTime = dt;    }
void Schedule::setArrivalTime(const string& at)   { arrivalTime   = at;    }
void Schedule::setDays(const string& d)           { days          = d;     }
