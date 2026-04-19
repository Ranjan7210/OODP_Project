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
    rows = read_csv(BUSES_CSV)
    for r in rows:
        try:
            r['capacity'] = int(r.get('capacity', 0))
        except (ValueError, TypeError):
            r['capacity'] = 0
    return rows


def read_stops():
    rows = read_csv(STOPS_CSV)
    for r in rows:
        try:
            r['latitude'] = float(r.get('latitude', 0))
            r['longitude'] = float(r.get('longitude', 0))
            r['routeCount'] = int(r.get('routeCount', 0))
        except (ValueError, TypeError):
            pass
    return rows


def read_routes():
    rows = read_csv(ROUTES_CSV)
    for r in rows:
        # stops are semicolon-separated in CSV
        raw = r.get('stops', '')
        r['stop_list'] = [s.strip() for s in raw.split(';') if s.strip()]
        try:
            r['totalDistance'] = float(r.get('totalDistance', 0))
        except (ValueError, TypeError):
            r['totalDistance'] = 0.0
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


def get_stats_data():
    """Shared helper to calculate all statistics and chart data."""
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

    # Health score calculation
    health_score = round((active_count / total_buses * 100)) if total_buses > 0 else 100

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
        'non_ac_count': nonac_count, # for index.html compatibility
        'nonac_count': nonac_count,  # for compatibility
        'active_count': active_count,
        'maint_count': maint_count,
        'total_distance': total_distance,
        'sync_time': datetime.now().strftime('%d %b %Y, %H:%M:%S'),
    }

    chart_data = {
        'total_buses': total_buses,
        'ac_count': ac_count,
        'non_ac_count': nonac_count,
        'nonac_count': nonac_count,
        'active_count': active_count,
        'maint_count': maint_count,
        'health_score': health_score,
        'stop_labels': stop_labels,
        'stop_counts': stop_counts,
        'route_labels': route_labels,
        'route_distances': route_distances,
    }

    # For stats.html route summary
    route_summary = []
    for r in routes:
        dist = 0
        try: dist = float(r.get('totalDistance', 0))
        except: pass
        coverage = min(100, (len(r.get('stop_list', [])) / 10 * 100)) if len(r.get('stop_list', [])) > 0 else 0
        eff = 'NORMAL'
        if dist > 30 and coverage > 80: eff = 'HIGH'
        elif dist < 10: eff = 'CRITICAL'
        
        route_summary.append({
            'routeNumber': r.get('routeNumber'),
            'name': r.get('name'),
            'distance': dist,
            'coverage': round(coverage),
            'efficiency': eff
        })

    return {
        'stats': stats,
        'chart_data': chart_data,
        'buses': buses,
        'stops': stops,
        'routes': routes,
        'schedules': schedules,
        'route_summary': route_summary
    }


# ============================================================
# LANDING PAGE
# ============================================================

@app.route('/')
def landing():
    data = get_stats_data()
    return render_template('landing.html', stats=data['stats'])


@app.route('/dashboard')
def dashboard():
    data = get_stats_data()
    return render_template('index.html', stats=data['stats'], chart_data=data['chart_data'])


@app.route('/data')
def data_explorer():
    data = get_stats_data()
    return render_template('data.html', 
                           buses=data['buses'], 
                           routes=data['routes'], 
                           stops=data['stops'], 
                           schedules=data['schedules'])


@app.route('/map')
def map_view():
    return render_template('map.html', api_key=None)


@app.route('/stats')
def stats_view():
    data = get_stats_data()
    return render_template('stats.html', 
                           chart_data=data['chart_data'], 
                           route_summary=data['route_summary'])


# ============================================================
# API ENDPOINTS (for Map and AJAX)
# ============================================================

@app.route('/api/stops')
def api_stops():
    stops = read_stops()
    # Format for map.html
    formatted = []
    for s in stops:
        try:
            formatted.append({
                'stopCode': s.get('id', ''),
                'name': s.get('name', ''),
                'lat': float(s.get('latitude', 19.0760)),
                'lng': float(s.get('longitude', 72.8777)),
                'routeCount': int(s.get('routeCount', 0))
            })
        except: pass
    return jsonify(formatted)


@app.route('/api/routes-geo')
def api_routes_geo():
    routes = read_routes()
    stops_data = {s['name']: s for s in read_stops()}
    
    formatted = []
    colors = ['#4F46E5', '#06B6D4', '#10B981', '#F59E0B', '#EF4444', '#8B5CF6']
    
    for i, r in enumerate(routes):
        path = []
        stop_names = r.get('stop_list', [])
        for name in stop_names:
            if name in stops_data:
                s = stops_data[name]
                try:
                    path.append({'lat': float(s.get('latitude')), 'lng': float(s.get('longitude'))})
                except: pass
        
        formatted.append({
            'routeNumber': r.get('routeNumber'),
            'name': r.get('name'),
            'totalDistance': r.get('totalDistance'),
            'color': colors[i % len(colors)],
            'path': path
        })
    return jsonify(formatted)


@app.route('/api/bus-locations')
def api_bus_locations():
    buses = read_buses()
    # Mock some movement/locations based on currentRouteId
    # In a real app, these would come from a GPS tracking table
    stops_data = {s['id']: s for s in read_stops()} # Using stop ID or Name
    
    formatted = []
    for b in buses:
        # Mocking lat/lng near Mumbai if not available
        lat = 19.0760 + (int(b.get('id', 0)) % 10) * 0.01
        lng = 72.8777 + (int(b.get('id', 0)) % 7) * 0.01
        
        formatted.append({
            'busNumber': b.get('busNumber'),
            'status': b.get('status'),
            'type': b.get('type'),
            'routeNumber': b.get('currentRouteId'),
            'lat': lat,
            'lng': lng
        })
    return jsonify(formatted)


# ============================================================
# ADMIN PORTAL — Routes
# ============================================================

@app.route('/admin/')
@app.route('/admin/dashboard')
def admin_dashboard():
    data = get_stats_data()
    return render_template('admin/dashboard.html',
                           stats=data['stats'],
                           chart_data=data['chart_data'],
                           buses=data['buses'],
                           schedules=data['schedules'])


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


@app.route('/admin/upload_buses', methods=['POST'])
def admin_upload_buses():
    """Bulk-import buses from an uploaded CSV file."""
    if 'csv_file' not in request.files:
        return jsonify({'success': False, 'message': 'No file uploaded.'}), 400

    file = request.files['csv_file']
    if not file or file.filename == '':
        return jsonify({'success': False, 'message': 'Empty file name.'}), 400
    if not file.filename.lower().endswith('.csv'):
        return jsonify({'success': False, 'message': 'Only CSV files are accepted.'}), 400

    try:
        content = file.read().decode('utf-8')
    except Exception as e:
        return jsonify({'success': False, 'message': f'Could not read file: {e}'}), 400

    import io
    reader = csv.DictReader(io.StringIO(content))

    existing_buses = read_buses()
    existing_numbers = {b.get('busNumber', '').strip().upper() for b in existing_buses}
    start_id = next_id(existing_buses)

    added = 0
    skipped = 0
    errors = []

    # Normalise header names (strip whitespace)
    fieldnames_required = {'busNumber'}
    allowed_fields = {'id', 'name', 'busNumber', 'capacity', 'type', 'status', 'currentRouteId'}

    for i, row in enumerate(reader, start=1):
        # Strip whitespace from keys and values
        row = {k.strip(): v.strip() for k, v in row.items() if k}

        bus_num = row.get('busNumber', '').strip()
        if not bus_num:
            errors.append(f'Row {i}: missing busNumber — skipped.')
            skipped += 1
            continue

        if bus_num.upper() in existing_numbers:
            errors.append(f'Row {i}: {bus_num} already exists — skipped.')
            skipped += 1
            continue

        try:
            cap = int(row.get('capacity', 0))
        except (ValueError, TypeError):
            cap = 0

        bus = {
            'id': start_id,
            'name': row.get('name', f'Bus {bus_num}'),
            'busNumber': bus_num,
            'capacity': cap,
            'type': row.get('type', 'Non-AC') if row.get('type', 'Non-AC') in ('AC', 'Non-AC') else 'Non-AC',
            'status': row.get('status', 'Active') if row.get('status', 'Active') in ('Active', 'Maintenance') else 'Active',
            'currentRouteId': row.get('currentRouteId', ''),
        }
        append_bus(bus)
        existing_numbers.add(bus_num.upper())
        start_id += 1
        added += 1

    message = f'{added} bus(es) added successfully.'
    if skipped:
        message += f' {skipped} row(s) skipped.'

    return jsonify({
        'success': True,
        'added': added,
        'skipped': skipped,
        'message': message,
        'errors': errors
    })


@app.route('/admin/upload_routes', methods=['POST'])
def admin_upload_routes():
    """Bulk-import routes from an uploaded CSV file."""
    if 'csv_file' not in request.files:
        return jsonify({'success': False, 'message': 'No file uploaded.'}), 400

    file = request.files['csv_file']
    if not file or file.filename == '':
        return jsonify({'success': False, 'message': 'Empty file name.'}), 400
    if not file.filename.lower().endswith('.csv'):
        return jsonify({'success': False, 'message': 'Only CSV files are accepted.'}), 400

    try:
        content = file.read().decode('utf-8')
    except Exception as e:
        return jsonify({'success': False, 'message': f'Could not read file: {e}'}), 400

    import io
    reader = csv.DictReader(io.StringIO(content))

    existing_routes = read_routes()
    existing_numbers = {r.get('routeNumber', '').strip().upper() for r in existing_routes}
    start_id = next_id(existing_routes)

    added = 0
    skipped = 0
    errors = []

    for i, row in enumerate(reader, start=1):
        row = {k.strip(): v.strip() for k, v in row.items() if k}

        route_num = row.get('routeNumber', '').strip()
        if not route_num:
            errors.append(f'Row {i}: missing routeNumber — skipped.')
            skipped += 1
            continue

        if route_num.upper() in existing_numbers:
            errors.append(f'Row {i}: {route_num} already exists — skipped.')
            skipped += 1
            continue

        try:
            dist = float(row.get('totalDistance', 0))
        except (ValueError, TypeError):
            dist = 0.0

        route = {
            'id': start_id,
            'name': row.get('name', f'Route {route_num}'),
            'routeNumber': route_num,
            'startStop': row.get('startStop', ''),
            'endStop': row.get('endStop', ''),
            'totalDistance': dist,
            'stops': row.get('stops', ''),
        }
        append_route(route)
        existing_numbers.add(route_num.upper())
        start_id += 1
        added += 1

    message = f'{added} route(s) added successfully.'
    if skipped:
        message += f' {skipped} row(s) skipped.'

    return jsonify({
        'success': True,
        'added': added,
        'skipped': skipped,
        'message': message,
        'errors': errors
    })


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


@app.route('/admin/upload_schedules', methods=['POST'])
def admin_upload_schedules():
    """Bulk-import schedules from an uploaded CSV file."""
    if 'csv_file' not in request.files:
        return jsonify({'success': False, 'message': 'No file uploaded.'}), 400

    file = request.files['csv_file']
    if not file or file.filename == '':
        return jsonify({'success': False, 'message': 'Empty file name.'}), 400
    if not file.filename.lower().endswith('.csv'):
        return jsonify({'success': False, 'message': 'Only CSV files are accepted.'}), 400

    try:
        content = file.read().decode('utf-8')
    except Exception as e:
        return jsonify({'success': False, 'message': f'Could not read file: {e}'}), 400

    import io
    reader = csv.DictReader(io.StringIO(content))

    existing_schedules = read_schedules()
    start_id = next_id(existing_schedules)

    added   = 0
    skipped = 0
    errors  = []

    for i, row in enumerate(reader, start=1):
        row = {k.strip(): v.strip() for k, v in row.items() if k}

        bus_id   = row.get('busId', '').strip()
        route_id = row.get('routeId', '').strip()

        if not bus_id:
            errors.append(f'Row {i}: missing busId — skipped.')
            skipped += 1
            continue
        if not route_id:
            errors.append(f'Row {i}: missing routeId — skipped.')
            skipped += 1
            continue

        dep = row.get('departureTime', '').strip()
        arr = row.get('arrivalTime', '').strip()

        if not dep or not arr:
            errors.append(f'Row {i}: missing departureTime or arrivalTime — skipped.')
            skipped += 1
            continue

        sched = {
            'id': start_id,
            'busId': bus_id,
            'routeId': route_id,
            'departureTime': dep,
            'arrivalTime': arr,
            'days': row.get('days', 'Daily').strip() or 'Daily',
        }
        append_schedule(sched)
        start_id += 1
        added += 1

    message = f'{added} schedule(s) added successfully.'
    if skipped:
        message += f' {skipped} row(s) skipped.'

    return jsonify({
        'success': True,
        'added': added,
        'skipped': skipped,
        'message': message,
        'errors': errors
    })


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
    app.run(debug=True, port=0) # Automatically find an available port preferred, but I'll use 5001 for consistency.
