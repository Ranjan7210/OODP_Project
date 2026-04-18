from flask import Flask, render_template, jsonify, request, redirect, url_for, flash
import csv
import os
from datetime import datetime

app = Flask(__name__)
app.secret_key = "oodp_bus_system_2024"

# ============================================================
# DATA DIRECTORY — shared with C++ terminal program via CSVs
# ============================================================
DATA_DIR = os.path.join(os.path.dirname(__file__), '..', 'data')

BUSES_CSV    = os.path.join(DATA_DIR, 'buses.csv')
STOPS_CSV    = os.path.join(DATA_DIR, 'stops.csv')
ROUTES_CSV   = os.path.join(DATA_DIR, 'routes.csv')
SCHEDULES_CSV = os.path.join(DATA_DIR, 'schedules.csv')


# ============================================================
# CSV HELPERS — read functions (used by both portals)
# ============================================================

def read_csv(filepath):
    """Read a CSV file and return a list of dicts. Returns [] on error."""
    rows = []
    try:
        with open(filepath, newline='', encoding='utf-8') as f:
            reader = csv.DictReader(f)
            for row in reader:
                rows.append(dict(row))
    except FileNotFoundError:
        pass
    return rows


def read_buses():
    return read_csv(BUSES_CSV)


def read_stops():
    return read_csv(STOPS_CSV)


def read_routes():
    rows = read_csv(ROUTES_CSV)
    for r in rows:
        # stops are semicolon-separated in CSV
        raw = r.get('stops', '')
        r['stop_list'] = [s.strip() for s in raw.split(';') if s.strip()]
    return rows


def read_schedules():
    return read_csv(SCHEDULES_CSV)


# ============================================================
# CSV WRITE HELPERS — used only by Admin portal
#
# NOTE (Academic Scope): In a production system, concurrent writes
# between the C++ process and this Flask server would require a
# proper database (e.g. SQLite with WAL mode, PostgreSQL) or a
# file-locking mechanism. For this academic project, we use simple
# CSV file appending which is safe only under single-writer assumptions.
# ============================================================

def append_bus(bus_dict):
    """Append a single bus row to buses.csv."""
    fieldnames = ['id', 'name', 'busNumber', 'capacity', 'type', 'status', 'currentRouteId']
    file_exists = os.path.isfile(BUSES_CSV)
    with open(BUSES_CSV, 'a', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        if not file_exists:
            writer.writeheader()
        writer.writerow(bus_dict)


def append_route(route_dict):
    """Append a single route row to routes.csv."""
    fieldnames = ['id', 'name', 'routeNumber', 'startStop', 'endStop', 'totalDistance', 'stops']
    file_exists = os.path.isfile(ROUTES_CSV)
    with open(ROUTES_CSV, 'a', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        if not file_exists:
            writer.writeheader()
        writer.writerow(route_dict)


def append_schedule(sched_dict):
    """Append a single schedule row to schedules.csv."""
    fieldnames = ['id', 'busId', 'routeId', 'departureTime', 'arrivalTime', 'days']
    file_exists = os.path.isfile(SCHEDULES_CSV)
    with open(SCHEDULES_CSV, 'a', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        if not file_exists:
            writer.writeheader()
        writer.writerow(sched_dict)


def next_id(rows):
    """Return next integer ID based on existing rows."""
    if not rows:
        return 1
    ids = []
    for r in rows:
        try:
            ids.append(int(r.get('id', 0)))
        except (ValueError, TypeError):
            pass
    return (max(ids) + 1) if ids else 1


# ============================================================
# LANDING PAGE
# ============================================================

@app.route('/')
def landing():
    return render_template('landing.html')


# ============================================================
# ADMIN PORTAL — Routes
# ============================================================

@app.route('/admin/')
@app.route('/admin/dashboard')
def admin_dashboard():
    buses     = read_buses()
    stops     = read_stops()
    routes    = read_routes()
    schedules = read_schedules()

    total_buses    = len(buses)
    total_stops    = len(stops)
    total_routes   = len(routes)
    total_schedules = len(schedules)

    ac_count    = sum(1 for b in buses if b.get('type') == 'AC')
    nonac_count = sum(1 for b in buses if b.get('type') == 'Non-AC')
    active_count = sum(1 for b in buses if b.get('status') == 'Active')
    maint_count  = sum(1 for b in buses if b.get('status') == 'Maintenance')

    total_distance = 0
    for r in routes:
        try:
            total_distance += float(r.get('totalDistance', 0))
        except (ValueError, TypeError):
            pass
    total_distance = round(total_distance, 1)

    # Busiest stops for bar chart (top 8)
    sorted_stops = sorted(stops, key=lambda s: int(s.get('routeCount', 0)), reverse=True)[:8]
    stop_labels  = [s.get('name', '') for s in sorted_stops]
    stop_counts  = [int(s.get('routeCount', 0)) for s in sorted_stops]

    # Route distance data for bar chart
    route_labels    = [r.get('routeNumber', '') for r in routes]
    route_distances = []
    for r in routes:
        try:
            route_distances.append(float(r.get('totalDistance', 0)))
        except (ValueError, TypeError):
            route_distances.append(0)

    stats = {
        'total_buses': total_buses,
        'total_stops': total_stops,
        'total_routes': total_routes,
        'total_schedules': total_schedules,
        'ac_count': ac_count,
        'nonac_count': nonac_count,
        'active_count': active_count,
        'maint_count': maint_count,
        'total_distance': total_distance,
        'sync_time': datetime.now().strftime('%d %b %Y, %H:%M:%S'),
    }

    chart_data = {
        'ac_count': ac_count,
        'nonac_count': nonac_count,
        'active_count': active_count,
        'maint_count': maint_count,
        'stop_labels': stop_labels,
        'stop_counts': stop_counts,
        'route_labels': route_labels,
        'route_distances': route_distances,
    }

    return render_template('admin/dashboard.html',
                           stats=stats,
                           chart_data=chart_data,
                           buses=buses,
                           schedules=schedules)


@app.route('/admin/buses')
def admin_buses():
    buses  = read_buses()
    routes = read_routes()
    return render_template('admin/buses.html', buses=buses, routes=routes)


@app.route('/admin/add_bus', methods=['POST'])
def admin_add_bus():
    # NOTE (Academic Scope): See write helper comment above regarding
    # concurrent write safety between C++ and Flask processes.
    buses = read_buses()
    new_id = next_id(buses)

    bus = {
        'id': new_id,
        'name': request.form.get('name', '').strip(),
        'busNumber': request.form.get('busNumber', '').strip(),
        'capacity': request.form.get('capacity', '0').strip(),
        'type': request.form.get('type', 'Non-AC').strip(),
        'status': request.form.get('status', 'Active').strip(),
        'currentRouteId': request.form.get('currentRouteId', '').strip(),
    }

    if not bus['busNumber']:
        flash('Bus number is required.', 'error')
        return redirect(url_for('admin_buses'))

    append_bus(bus)
    flash(f"Bus {bus['busNumber']} added successfully!", 'success')
    return redirect(url_for('admin_buses'))


@app.route('/admin/routes')
def admin_routes():
    routes = read_routes()
    return render_template('admin/routes.html', routes=routes)


@app.route('/admin/add_route', methods=['POST'])
def admin_add_route():
    # NOTE (Academic Scope): See write helper comment above.
    routes = read_routes()
    new_id = next_id(routes)

    stops_raw = request.form.get('stops', '').strip()

    route = {
        'id': new_id,
        'name': request.form.get('name', '').strip(),
        'routeNumber': request.form.get('routeNumber', '').strip(),
        'startStop': request.form.get('startStop', '').strip(),
        'endStop': request.form.get('endStop', '').strip(),
        'totalDistance': request.form.get('totalDistance', '0').strip(),
        'stops': stops_raw,
    }

    if not route['routeNumber']:
        flash('Route number is required.', 'error')
        return redirect(url_for('admin_routes'))

    append_route(route)
    flash(f"Route {route['routeNumber']} added successfully!", 'success')
    return redirect(url_for('admin_routes'))


@app.route('/admin/schedules')
def admin_schedules():
    schedules = read_schedules()
    buses     = read_buses()
    routes    = read_routes()
    return render_template('admin/schedules.html',
                           schedules=schedules, buses=buses, routes=routes)


@app.route('/admin/add_schedule', methods=['POST'])
def admin_add_schedule():
    # NOTE (Academic Scope): See write helper comment above.
    schedules = read_schedules()
    new_id = next_id(schedules)

    sched = {
        'id': new_id,
        'busId': request.form.get('busId', '').strip(),
        'routeId': request.form.get('routeId', '').strip(),
        'departureTime': request.form.get('departureTime', '').strip(),
        'arrivalTime': request.form.get('arrivalTime', '').strip(),
        'days': request.form.get('days', '').strip(),
    }

    append_schedule(sched)
    flash('Schedule added successfully!', 'success')
    return redirect(url_for('admin_schedules'))


# ============================================================
# CITIZEN PORTAL — Read-Only Routes
# ============================================================

@app.route('/citizen/')
@app.route('/citizen/home')
def citizen_home():
    buses     = read_buses()
    routes    = read_routes()
    stops     = read_stops()
    schedules = read_schedules()

    active_buses   = [b for b in buses if b.get('status') == 'Active']
    total_routes   = len(routes)
    total_stops    = len(stops)
    total_schedules = len(schedules)

    return render_template('citizen/home.html',
                           active_buses=active_buses,
                           total_routes=total_routes,
                           total_stops=total_stops,
                           total_schedules=total_schedules,
                           buses=buses)


@app.route('/citizen/routes')
def citizen_routes():
    routes = read_routes()
    query  = request.args.get('q', '').strip().lower()

    if query:
        filtered = []
        for r in routes:
            name = r.get('name', '').lower()
            num  = r.get('routeNumber', '').lower()
            stops_str = r.get('stops', '').lower()
            if query in name or query in num or query in stops_str:
                filtered.append(r)
        routes = filtered

    return render_template('citizen/routes.html', routes=routes, query=query)


@app.route('/citizen/schedules')
def citizen_schedules():
    schedules = read_schedules()
    buses     = read_buses()
    routes    = read_routes()

    # Build lookup maps for display
    bus_map   = {b['busNumber']: b for b in buses if 'busNumber' in b}
    route_map = {r['routeNumber']: r for r in routes if 'routeNumber' in r}

    enriched = []
    for s in schedules:
        bus_id   = s.get('busId', '')
        route_id = s.get('routeId', '')
        bus_info   = bus_map.get(bus_id, {})
        route_info = route_map.get(route_id, {})
        enriched.append({
            **s,
            'busType':    bus_info.get('type', '—'),
            'busStatus':  bus_info.get('status', '—'),
            'routeName':  route_info.get('name', route_id),
            'startStop':  route_info.get('startStop', '—'),
            'endStop':    route_info.get('endStop', '—'),
        })

    return render_template('citizen/schedules.html', schedules=enriched)


# ============================================================
# Run
# ============================================================
if __name__ == '__main__':
    app.run(debug=True, port=5000)
