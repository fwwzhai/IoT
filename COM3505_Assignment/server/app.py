from collections import deque
from datetime import datetime, timezone
import os

from flask import Flask, jsonify, render_template, request

app = Flask(__name__)
app.config["SEND_FILE_MAX_AGE_DEFAULT"] = 0

ASSET_VERSION = "20260503-pattern-pill"
MAX_HISTORY_POINTS = 120
VALID_PATTERNS = {"blink", "chase", "cycle", "alert", "pulse", "heartbeat"}
VALID_MODES = {"manual", "auto"}
TEMPERATURE_WARNING_C = 18.0
TEMPERATURE_ALERT_C = 24.0

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
        "button_pressed": None,
    },
}
HISTORY = deque(maxlen=MAX_HISTORY_POINTS)


def utc_now_iso():
    return datetime.now(timezone.utc).isoformat()


@app.after_request
def add_no_cache_headers(response):
    response.headers["Cache-Control"] = "no-store, no-cache, must-revalidate, max-age=0"
    response.headers["Pragma"] = "no-cache"
    response.headers["Expires"] = "0"
    return response


def sensor_temperature():
    value = STATE["sensors"].get("temperature_c")
    if value is None:
        return None

    try:
        return float(value)
    except (TypeError, ValueError):
        return None


def apply_auto_pattern():
    if STATE["device"]["mode"] != "auto":
        return

    temperature_c = sensor_temperature()
    button_pressed = STATE["sensors"].get("button_pressed") is True

    if button_pressed or (
        temperature_c is not None and temperature_c >= TEMPERATURE_ALERT_C
    ):
        STATE["device"]["pattern"] = "alert"
        return

    if temperature_c is not None and temperature_c >= TEMPERATURE_WARNING_C:
        STATE["device"]["pattern"] = "pulse"
        return

    STATE["device"]["pattern"] = "blink"


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
    return render_template("index.html", asset_version=ASSET_VERSION)


@app.get("/api/state")
def get_state():
    apply_auto_pattern()
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
    STATE["sensors"]["button_pressed"] = sensors.get("button_pressed")

    if "wifi_connected" in device:
        STATE["device"]["wifi_connected"] = device["wifi_connected"]
    if "last_seen" in device:
        STATE["device"]["last_seen"] = device["last_seen"]
    else:
        STATE["device"]["last_seen"] = utc_now_iso()

    apply_auto_pattern()

    HISTORY.append(
        {
            "timestamp": STATE["device"]["last_seen"],
            "temperature_c": STATE["sensors"]["temperature_c"],
            "button_pressed": STATE["sensors"]["button_pressed"],
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
    if pattern in VALID_PATTERNS and STATE["device"]["mode"] != "auto":
        STATE["device"]["pattern"] = pattern
    apply_auto_pattern()
    return jsonify({"ok": True, "pattern": STATE["device"]["pattern"]})


@app.post("/api/mode")
def post_mode():
    payload = request.get_json(silent=True) or {}
    mode = payload.get("mode")
    if mode in VALID_MODES:
        STATE["device"]["mode"] = mode
    apply_auto_pattern()
    return jsonify({"ok": True, "mode": STATE["device"]["mode"]})


if __name__ == "__main__":
    host = os.getenv("APP_HOST", "0.0.0.0")
    port = int(os.getenv("APP_PORT", "5000"))
    app.run(host=host, port=port, debug=True)
