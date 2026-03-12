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
