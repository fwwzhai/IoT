# COM3505 Assignment Workspace

This folder is the working tree for the assignment build.

The structure follows the lab-style PlatformIO layout for firmware, with separate folders for the Flask server and submission assets.

## Layout

- `firmware/` ESP32 PlatformIO project
- `server/` Flask backend and dashboard
- `docs/` report, diagrams, and demo assets

## Firmware Layout

- `firmware/src/main.cpp` Arduino entry point only
- `firmware/src/Thing.cpp` top-level device orchestration and shared state
- `firmware/src/Sensors.cpp` sensor sampling
- `firmware/src/Patterns.cpp` LED pattern engine
- `firmware/src/Network.cpp` Wi-Fi and server sync scaffolding
- `firmware/include/` shared headers, config, and pin mappings

## Notes

- This workspace is intentionally separate from the lab exercises.
- The labs are reference material for style and learned techniques only.
- The product direction is defined in the repo-level `ASSIGNMENT_PRD.md`.

## Quick Start

### Flask dashboard

1. Open `COM3505_Assignment/server`
2. Install dependencies:
   `python -m pip install --user -r requirements.txt`
3. Run the server:
   `python app.py`
4. Open:
   `http://127.0.0.1:5000`

### ESP32 firmware

1. Copy `firmware/include/Secrets.example.h` to `firmware/include/Secrets.h`
2. Set:
   - `kHasRealWifiCredentials = true`
   - your Wi-Fi SSID/password
   - your laptop IPv4 address as `SERVER_HOST`
3. Wire the current bring-up hardware:
   - TMP36 middle leg -> `A0`
   - red LED -> `GPIO6`
   - yellow LED -> `GPIO9`
   - green LED -> `GPIO12`
4. Build from `COM3505_Assignment/firmware`:
   `pio run`

## Current Bring-Up Mode

- The firmware is currently in `TMP36-only first-test mode`
- Light and motion inputs are stubbed during bring-up
- Dashboard graph, controls, and server sync are live
