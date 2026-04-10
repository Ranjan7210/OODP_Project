from flask import Flask, render_template

app = Flask(__name__)

@app.route('/')
def index():
    # Mock statistics for dashboard
    stats = {
        'total_buses': 42,
        'active_routes': 15,
        'daily_trips': 128,
        'fleet_efficiency': '94%'
    }
    return render_template('index.html', stats=stats)

@app.route('/data')
def data():
    # Mock data for tables
    schedules = [
        {'id': 'B-101', 'route': 'Downtown - Airport', 'dep': '08:00 AM', 'arr': '08:45 AM', 'status': 'On Time', 'status_class': 'success'},
        {'id': 'B-102', 'route': 'Westside - Central', 'dep': '08:15 AM', 'arr': '09:00 AM', 'status': 'Delayed', 'status_class': 'warning'},
        {'id': 'B-103', 'route': 'North - South Express', 'dep': '08:30 AM', 'arr': '09:30 AM', 'status': 'On Time', 'status_class': 'success'},
        {'id': 'B-104', 'route': 'Eastside Loop', 'dep': '09:00 AM', 'arr': '09:45 AM', 'status': 'Maintenance', 'status_class': 'danger'},
    ]
    return render_template('data.html', schedules=schedules)

# This is for local development only
if __name__ == '__main__':
    app.run(debug=True, port=5000)
