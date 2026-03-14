from collections import deque
from datetime import datetime, timezone

from flask import Flask, jsonify, render_template, request

app = Flask(__name__)
MAX_HISTORY_POINTS = 120
VALID_PATTERNS = {"blink", "chase", "cycle", "alert"}
VALID_MODES = {"manual", "auto"}

STATE = {
    "device": {
        "mode": "manual",
        "pattern": "blink",
        "wifi_connected": False,
        "last_seen": None,
        "health": "offline",
    },
    "sensors": {
        "temperature_c": None,
        "light_level": None,
        "motion_detected": None,
    },
}
HISTORY = deque(maxlen=MAX_HISTORY_POINTS)


def utc_now_iso():
    return datetime.now(timezone.utc).isoformat()


def update_health_state():
    last_seen = STATE["device"]["last_seen"]
    if not last_seen:
        STATE["device"]["health"] = "offline"
        return

    try:
        last_seen_dt = datetime.fromisoformat(last_seen)
    except ValueError:
        STATE["device"]["health"] = "offline"
        return

    age_seconds = (datetime.now(timezone.utc) - last_seen_dt).total_seconds()
    if age_seconds <= 6:
        STATE["device"]["health"] = "live"
    elif age_seconds <= 20:
        STATE["device"]["health"] = "stale"
    else:
        STATE["device"]["health"] = "offline"


def snapshot_history():
    return list(HISTORY)


@app.get("/")
def index():
    return render_template("index.html")


@app.get("/api/state")
def get_state():
    update_health_state()
    return jsonify(STATE)


@app.get("/api/history")
def get_history():
    return jsonify({"history": snapshot_history()})


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
    else:
        STATE["device"]["last_seen"] = utc_now_iso()

    HISTORY.append(
        {
            "timestamp": STATE["device"]["last_seen"],
            "temperature_c": STATE["sensors"]["temperature_c"],
            "light_level": STATE["sensors"]["light_level"],
            "motion_detected": STATE["sensors"]["motion_detected"],
        }
    )
    update_health_state()

    return jsonify({"ok": True})


@app.get("/api/health")
def health():
    return jsonify({"ok": True, "service": "flask-dashboard"})


@app.post("/api/pattern")
def post_pattern():
    payload = request.get_json(silent=True) or {}
    pattern = payload.get("pattern")
    if pattern in VALID_PATTERNS:
        STATE["device"]["pattern"] = pattern
    return jsonify({"ok": True, "pattern": STATE["device"]["pattern"]})


@app.post("/api/mode")
def post_mode():
    payload = request.get_json(silent=True) or {}
    mode = payload.get("mode")
    if mode in VALID_MODES:
        STATE["device"]["mode"] = mode
    return jsonify({"ok": True, "mode": STATE["device"]["mode"]})


if __name__ == "__main__":
    app.run(host="0.0.0.0", debug=True)
