from flask import Flask, render_template, jsonify, request
import pandas as pd
import os
from datetime import datetime
import time
import math

app = Flask(__name__)

# Google Maps API Configuration
# Add your API key below from https://console.cloud.google.com/
GOOGLE_MAPS_API_KEY = "YOUR_API_KEY_HERE"

# ============================================================
# Helper — load CSV from the data/ folder (one level up)
# ============================================================
def load_csv(filename):
    """Load a CSV file from ../data/ folder. Returns empty DataFrame on error."""
    filepath = os.path.join(os.path.dirname(__file__), '..', 'data', filename)
    try:
        df = pd.read_csv(filepath)
        return df
    except Exception:
        return pd.DataFrame()


# ============================================================
# Route: / — Dashboard Home
# ============================================================
@app.route('/')
def index():
    buses = load_csv('buses.csv')
    stops = load_csv('stops.csv')
    routes = load_csv('routes.csv')
    schedules = load_csv('schedules.csv')

    # Summary stats
    total_buses = len(buses)
    total_stops = len(stops)
    total_routes = len(routes)
    total_distance = round(routes['totalDistance'].sum(), 1) if not routes.empty and 'totalDistance' in routes.columns else 0

    ac_count = len(buses[buses['type'] == 'AC']) if not buses.empty and 'type' in buses.columns else 0
    non_ac_count = len(buses[buses['type'] == 'Non-AC']) if not buses.empty and 'type' in buses.columns else 0

    active_count = len(buses[buses['status'] == 'Active']) if not buses.empty and 'status' in buses.columns else 0
    maintenance_count = len(buses[buses['status'] == 'Maintenance']) if not buses.empty and 'status' in buses.columns else 0

    stats = {
        'total_buses': total_buses,
        'total_stops': total_stops,
        'total_routes': total_routes,
        'total_distance': total_distance,
        'ac_count': ac_count,
        'non_ac_count': non_ac_count,
        'active_count': active_count,
        'maintenance_count': maintenance_count,
        'sync_time': datetime.now().strftime('%H:%M:%S')
    }

    return render_template('index.html', stats=stats)


# ============================================================
# Route: /data — Data Management Tables
# ============================================================
@app.route('/data')
def data():
    buses = load_csv('buses.csv')
    stops = load_csv('stops.csv')
    routes_df = load_csv('routes.csv')
    schedules = load_csv('schedules.csv')

    # Convert to list of dicts for Jinja
    buses_data = buses.to_dict('records') if not buses.empty else []
    stops_data = stops.to_dict('records') if not stops.empty else []
    routes_data = routes_df.to_dict('records') if not routes_df.empty else []
    schedules_data = schedules.to_dict('records') if not schedules.empty else []

    # Compute summary metrics for bottom cards
    active_count = len(buses[buses['status'] == 'Active']) if not buses.empty and 'status' in buses.columns else 0
    total_buses = len(buses)
    fleet_health = round((active_count / total_buses * 100), 1) if total_buses > 0 else 0

    summary = {
        'fleet_health': fleet_health,
        'total_routes': len(routes_df),
        'total_buses': total_buses,
    }

    return render_template('data.html',
                           buses=buses_data,
                           stops=stops_data,
                           routes=routes_data,
                           schedules=schedules_data,
                           summary=summary)


# ============================================================
# Route: /stats — Fleet Analytics with Chart Data
# ============================================================
@app.route('/stats')
def stats():
    buses = load_csv('buses.csv')
    stops = load_csv('stops.csv')
    routes_df = load_csv('routes.csv')
    schedules = load_csv('schedules.csv')

    # Chart 1: Route Distances (Bar chart)
    route_labels = routes_df['routeNumber'].tolist() if not routes_df.empty and 'routeNumber' in routes_df.columns else []
    route_distances = routes_df['totalDistance'].tolist() if not routes_df.empty and 'totalDistance' in routes_df.columns else []

    # Chart 2: AC vs Non-AC (Pie chart)
    ac_count = len(buses[buses['type'] == 'AC']) if not buses.empty and 'type' in buses.columns else 0
    non_ac_count = len(buses[buses['type'] == 'Non-AC']) if not buses.empty and 'type' in buses.columns else 0

    # Chart 3: Top 5 Busiest Stops (Horizontal bar)
    if not stops.empty and 'routeCount' in stops.columns:
        top_stops = stops.nlargest(5, 'routeCount')
        stop_labels = top_stops['name'].tolist()
        stop_counts = top_stops['routeCount'].tolist()
    else:
        stop_labels = []
        stop_counts = []

    # Chart 4: Active vs Maintenance (Doughnut)
    active_count = len(buses[buses['status'] == 'Active']) if not buses.empty and 'status' in buses.columns else 0
    maint_count = len(buses[buses['status'] == 'Maintenance']) if not buses.empty and 'status' in buses.columns else 0

    # Network health score
    total_buses = len(buses)
    health_score = round((active_count / total_buses * 100), 1) if total_buses > 0 else 0

    # Route summary table data
    route_summary = []
    if not routes_df.empty:
        max_dist = routes_df['totalDistance'].max() if 'totalDistance' in routes_df.columns else 1
        for _, row in routes_df.iterrows():
            dist = row.get('totalDistance', 0)
            coverage = round((dist / max_dist * 100), 0) if max_dist > 0 else 0
            efficiency = 'HIGH' if coverage >= 80 else ('NORMAL' if coverage >= 50 else 'CRITICAL')
            route_summary.append({
                'routeNumber': row.get('routeNumber', ''),
                'name': row.get('name', ''),
                'distance': dist,
                'coverage': coverage,
                'efficiency': efficiency
            })

    chart_data = {
        'route_labels': route_labels,
        'route_distances': route_distances,
        'ac_count': ac_count,
        'non_ac_count': non_ac_count,
        'stop_labels': stop_labels,
        'stop_counts': stop_counts,
        'active_count': active_count,
        'maint_count': maint_count,
        'total_buses': total_buses,
        'health_score': health_score,
    }

    return render_template('stats.html',
                           chart_data=chart_data,
                           route_summary=route_summary)


# ============================================================
# API Route: /api/routes — JSON endpoint
# ============================================================
@app.route('/api/routes')
def api_routes():
    routes_df = load_csv('routes.csv')
    if routes_df.empty:
        return jsonify([])
    return jsonify(routes_df.to_dict('records'))


# ============================================================
# MAPS INTEGRATION: Page & APIs
# ============================================================

@app.route('/map')
def map_page():
    return render_template('map.html', api_key=GOOGLE_MAPS_API_KEY)


@app.route('/api/stops')
def api_stops():
    stops_df = load_csv('stops.csv')
    if stops_df.empty:
        return jsonify([])
    return jsonify(stops_df.to_dict('records'))


@app.route('/api/routes-geo')
def api_routes_geo():
    routes_df = load_csv('routes.csv')
    stops_df = load_csv('stops.csv')
    
    if routes_df.empty or stops_df.empty:
        return jsonify({})

    # Build stop lookup table (name -> {lat, lng, code})
    stop_lookup = {}
    for _, stop in stops_df.iterrows():
        stop_lookup[stop['name']] = {
            'lat': stop['latitude'],
            'lng': stop['longitude'],
            'stopCode': stop['stopCode'],
            'name': stop['name']
        }
        
    routes_geo = []
    colors = ['#4F46E5', '#06B6D4', '#10B981', '#F59E0B', '#EF4444']
    
    for i, row in routes_df.iterrows():
        route_num = row.get('routeNumber', '')
        stop_list_str = row.get('stops', '')
        if not isinstance(stop_list_str, str) or not stop_list_str:
            continue
            
        stops_in_route = stop_list_str.split(';')
        path = []
        for s_name in stops_in_route:
            if s_name in stop_lookup:
                path.append(stop_lookup[s_name])
                
        # Assign color cyclically
        color = colors[i % len(colors)]
        
        routes_geo.append({
            'routeNumber': route_num,
            'name': row.get('name', ''),
            'totalDistance': row.get('totalDistance', 0),
            'startStop': row.get('startStop', ''),
            'endStop': row.get('endStop', ''),
            'color': color,
            'path': path
        })
        
    return jsonify(routes_geo)

def simulate_bus_position(path, t):
    """Interpolate coordinates along a path given time t (0 to 1 looping)"""
    if not path or len(path) == 0:
        return None
    if len(path) == 1:
        return {'lat': path[0]['lat'], 'lng': path[0]['lng']}
        
    # Scale t to number of segments (n stops = n-1 segments)
    num_segments = len(path) - 1
    scaled_t = t * num_segments
    
    segment_idx = int(math.floor(scaled_t))
    if segment_idx >= num_segments:
        segment_idx = num_segments - 1
        
    # Fractional part decides how far along the current segment the bus is
    fraction = scaled_t - segment_idx
    
    start_point = path[segment_idx]
    end_point = path[segment_idx + 1]
    
    interp_lat = start_point['lat'] + (end_point['lat'] - start_point['lat']) * fraction
    interp_lng = start_point['lng'] + (end_point['lng'] - start_point['lng']) * fraction
    
    return {'lat': interp_lat, 'lng': interp_lng}


@app.route('/api/bus-locations')
def api_bus_locations():
    buses_df = load_csv('buses.csv')
    routes_geo_resp = api_routes_geo() 
    if buses_df.empty or getattr(routes_geo_resp, 'json', None) is None:
        return jsonify([])
        
    routes_geo = routes_geo_resp.json
    routes_dict = {r['routeNumber']: r for r in routes_geo}

    # Common time counter, looped. speed=0.0001 (approx 100 sec loop per segment roughly)
    current_t = (time.time() * 0.0001) % 1
        
    live_buses = []
    
    for _, bus in buses_df.iterrows():
        route_id = bus.get('currentRouteId')
        status = bus.get('status')
        bus_num = bus.get('busNumber')
        bus_type = bus.get('type')
        
        # Add basic info
        bus_info = {
            'busNumber': bus_num,
            'type': bus_type,
            'status': status,
            'routeNumber': route_id,
            'lat': None,
            'lng': None
        }
        
        # If it's on a known route, simulate location
        if route_id in routes_dict:
            route_path = routes_dict[route_id]['path']
            # Add a stable, arbitrary offset so all buses aren't in the exact same spot 
            offset = hash(bus_num) % 1000 / 1000.0  
            local_t = (current_t + offset) % 1
            
            # If in maintenance, we'll just freeze it at exactly its offset time
            if status == 'Maintenance':
                pos = simulate_bus_position(route_path, offset)
            else:
                pos = simulate_bus_position(route_path, local_t)
                
            if pos:
                bus_info['lat'] = pos['lat']
                bus_info['lng'] = pos['lng']
                
        live_buses.append(bus_info)
        
    return jsonify(live_buses)

# ============================================================
# Run
# ============================================================
if __name__ == '__main__':
    app.run(debug=True, port=5000)
