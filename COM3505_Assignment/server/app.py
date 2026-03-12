from flask import Flask, jsonify, render_template, request

app = Flask(__name__)

STATE = {
    "device": {
        "mode": "manual",
        "pattern": "blink",
        "wifi_connected": False,
        "last_seen": None,
    },
    "sensors": {
        "temperature_c": None,
        "light_level": None,
        "motion_detected": None,
    },
}


@app.get("/")
def index():
    return render_template("index.html")


@app.get("/api/state")
def get_state():
    return jsonify(STATE)


@app.post("/api/sensor")
def post_sensor():
    payload = request.get_json(silent=True) or {}
    sensors = payload.get("sensors", {})
    device = payload.get("device", {})

    STATE["sensors"]["temperature_c"] = sensors.get("temperature_c")
    STATE["sensors"]["light_level"] = sensors.get("light_level")
    STATE["sensors"]["motion_detected"] = sensors.get("motion_detected")

    if "mode" in device:
        STATE["device"]["mode"] = device["mode"]
    if "pattern" in device:
        STATE["device"]["pattern"] = device["pattern"]
    if "wifi_connected" in device:
        STATE["device"]["wifi_connected"] = device["wifi_connected"]
    if "last_seen" in device:
        STATE["device"]["last_seen"] = device["last_seen"]

    return jsonify({"ok": True})


@app.post("/api/pattern")
def post_pattern():
    payload = request.get_json(silent=True) or {}
    pattern = payload.get("pattern")
    if pattern:
        STATE["device"]["pattern"] = pattern
    return jsonify({"ok": True, "pattern": STATE["device"]["pattern"]})


@app.post("/api/mode")
def post_mode():
    payload = request.get_json(silent=True) or {}
    mode = payload.get("mode")
    if mode:
        STATE["device"]["mode"] = mode
    return jsonify({"ok": True, "mode": STATE["device"]["mode"]})


if __name__ == "__main__":
    app.run(debug=True)

