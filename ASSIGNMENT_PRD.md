# COM3505 Assignment PRD

Date: 2026-03-12
Module: COM3505 Internet of Things
Target: 90+ submission
Status: Working product requirements document

## 1. Purpose

This document captures the assignment direction agreed so far and turns it into a practical build specification.

The assignment will not reuse the current lab code directly. The labs are being treated as evidence of module style and of the techniques learned so far:

- GPIO and basic embedded control
- multi-LED state logic
- debug and loop timing discipline
- Wi-Fi connectivity
- HTTP communication
- browser-based monitoring
- analog sensor reading

## 2. Assignment Alignment

The assignment brief requires an ESP32-based IoT system that:

- reads data from at least one sensor
- controls at least three LEDs with dynamic flashing patterns
- connects to Wi-Fi
- sends sensor data to a Python Flask server
- shows live data in a browser interface
- allows LED control through the web interface

Clarifications from the assignment Q&A:

- RGB LEDs are not required
- three separate LEDs are acceptable
- AJAX polling is sufficient and fully acceptable
- WebSockets are allowed but optional
- extra components may be allowed, but students should not need to buy components themselves

## 3. Product Concept

Working concept name:

Smart Ambient Monitoring Node

Possible application framing:

- farm shed monitor
- greenhouse node
- room or workspace environmental monitor

Important framing note:

The inspiration may be "Echo Dot"-like in the sense of a compact smart node with a clean interface, but this is not a voice assistant project. For assessment purposes it should be presented as a modern sensor-monitoring and status-control IoT system.

## 4. Product Vision

Build a polished, modern IoT monitoring node that senses the local environment, reports live data to a Flask backend, and provides a responsive web dashboard for monitoring and LED mode control.

The system should feel deliberate, fast, and integrated, not like disconnected lab exercises.

## 5. Core Goals

- Deliver a stable end-to-end IoT system that clearly exceeds the basic assignment requirements.
- Produce a dashboard that feels modern and responsive.
- Show strong engineering decisions in timing, architecture, and system integration.
- Make the report easy to write by choosing a coherent product story early.

## 6. Non-Goals

- No voice recognition or speech synthesis
- No attempt to clone Alexa functionality
- No dependence on unfinished lab code
- No unnecessary complexity on the ESP32 side if it risks reliability

## 7. Recommended Hardware Scope

### Core hardware

- 1 x ESP32 Feather board
- 3 x LEDs
  - red
  - yellow
  - green
- 1 x temperature sensor
  - preferred: TMP36 or thermistor-based temperature measurement
- 1 x light sensor
  - preferred: LDR

### Optional hardware

- motion sensor, only if one is actually available through the kit or department

### Scope rule

Temperature plus light will be the guaranteed core. Motion will be treated as an optional enhancement, not as a dependency for the main product.

## 8. User Story

As a user, I want to open a browser dashboard and immediately see the environmental state around the device, understand whether conditions are normal or abnormal, and change LED modes manually or let the system react automatically.

## 9. High-Level System Design

### ESP32 responsibilities

- connect to local Wi-Fi
- read sensors on a schedule
- run LED patterns continuously using non-blocking timing
- send sensor data to Flask every 2 to 5 seconds
- fetch the latest control state from Flask
- expose useful serial diagnostics

### Flask responsibilities

- receive and store latest sensor values
- store current LED mode and automation settings
- serve the dashboard
- expose JSON APIs for device and browser communication
- optionally provide a live stream channel for browser updates

### Browser responsibilities

- display current readings clearly
- display device state and current LED mode
- allow manual pattern selection
- allow switching between manual and auto modes
- display recent sensor history
- remain responsive without full page refresh

## 10. Product Modes

### Manual mode

The user selects the LED pattern directly from the dashboard.

### Auto mode

The server or firmware applies rule-based behavior using sensor thresholds. Example:

- low light -> soft warning pattern
- high temperature -> alert pattern
- normal conditions -> calm status pattern

## 11. Functional Requirements

### FR1. Sensor telemetry

- The system must read at least one sensor.
- The target system should read both temperature and light if hardware permits.
- Sensor data must be sent to Flask at a regular interval between 2 and 5 seconds.

### FR2. LED pattern engine

- The system must control at least 3 LEDs.
- LEDs must support dynamic patterns, not just static on/off states.
- The project target is 4 patterns:
  - blink
  - chase
  - alternate cycle
  - flicker or alert pulse

### FR3. Web control

- The dashboard must let the user change LED patterns.
- The dashboard must show the current active mode and pattern.

### FR4. Live updates

- Sensor values must update live in the browser.
- The page must not rely on manual refresh.

### FR5. Network behavior

- ESP32 must connect to Wi-Fi in station mode.
- The serial monitor must show IP and connection status.

### FR6. Flask integration

- Flask must receive sensor values from ESP32.
- Flask must act as the central coordination layer between device and browser.

## 12. Stretch Requirements For 90+

- manual and auto mode support
- recent history graph for temperature and light
- responsive mobile-friendly layout
- robust reconnection behavior
- clear device health indicator on the dashboard
- sensor-driven LED automation with justified thresholds

## 13. UX Requirements

The UI must not feel slow, clunky, or lab-like.

Required characteristics:

- instant visual response on button presses
- clean information hierarchy
- clear status indicators
- modern styling
- smooth updates without full refresh
- readable on both desktop and mobile

Preferred approach:

- browser-to-Flask updates may use WebSockets if time allows
- AJAX polling remains acceptable and compliant
- ESP32-to-Flask should remain simple and reliable using HTTP + JSON

Recommended protocol choice:

- ESP32 <-> Flask: HTTP polling / JSON
- Browser <-> Flask: WebSocket if stable, otherwise AJAX polling

This keeps the firmware simple while allowing a more modern dashboard experience.

## 14. Engineering Requirements

- Firmware must avoid heavy use of `delay()` in the final design.
- Timing should be handled using `millis()` scheduling.
- Sensor sampling, animation, and networking should be separated logically.
- Wi-Fi credentials must not be hardcoded into version-controlled source without a safer config strategy.
- Code should be clearly structured and named for report readability.

## 15. Proposed Internal State Model

### Firmware state

- current pattern
- current mode
- latest temperature
- latest light level
- last animation timestamp
- last sensor upload timestamp
- last server sync timestamp
- connection status

### Server state

- latest readings
- recent reading history
- active mode
- active pattern
- threshold values
- last device seen timestamp

## 16. Proposed API Shape

These endpoints are provisional and may evolve:

- `GET /`
  - serve dashboard
- `GET /api/state`
  - current mode, pattern, thresholds, connection metadata
- `POST /api/sensor`
  - ESP32 uploads latest readings
- `POST /api/pattern`
  - browser sets manual pattern
- `POST /api/mode`
  - browser switches between manual and auto
- `GET /api/history`
  - recent values for charts

Optional if WebSockets are used:

- `/ws`
  - push live dashboard updates

## 17. LED Behavior Strategy

The LED system should look intentional rather than decorative.

Suggested meanings:

- green bias = healthy / normal
- yellow bias = warning / low-light / transition state
- red bias = alert / threshold exceeded

Suggested pattern mapping:

- normal: slow alternating or calm pulse
- warning: chase or alternating yellow emphasis
- alert: fast red pulse or flicker
- manual: user-selected pattern from UI

## 18. Why This Can Score 90+

This concept supports top-band performance because it:

- fully meets the brief
- has a coherent real-world use case
- combines sensing, actuation, networking, and UI into one product
- supports a polished dashboard rather than a minimal control page
- allows strong report discussion of timing, data flow, state, and system architecture
- naturally supports one or two meaningful enhancements without becoming chaotic

## 19. Risks And Controls

### Risk: motion sensor not available

Control:

- keep motion as optional
- do not depend on it for the core submission

### Risk: WebSockets consume too much build time

Control:

- keep AJAX fallback fully acceptable
- prioritize clean UI over protocol novelty

### Risk: blocking firmware causes lag

Control:

- implement scheduling with `millis()`
- keep pattern stepping non-blocking

### Risk: scope creep

Control:

- finish end-to-end core system before adding enhancements

## 20. Delivery Strategy

### Phase 1: lock the design

- confirm available sensors
- confirm LED pin choices
- confirm whether motion is possible
- lock API contract

### Phase 2: core backend and firmware

- Flask app skeleton
- sensor upload endpoint
- control state endpoint
- ESP32 Wi-Fi + sensor read + upload
- 3 working LED patterns

### Phase 3: dashboard

- live value cards
- pattern and mode controls
- update loop
- device health / last seen

### Phase 4: high-band polish

- auto mode
- history graph
- responsive design
- reconnect/error handling

### Phase 5: submission assets

- hardware diagram
- system architecture diagram
- concise report
- short demo video

## 21. Discussion Summary Captured From This Planning Session

- The current labs are being used only to understand module style and learning progression.
- The assignment should be built cleanly from scratch.
- The target is not just "good enough"; the target is 90+.
- The project should feel modern and responsive, not like a crude lab dashboard.
- A smart ambient monitor / smart farm node concept is well aligned with the brief.
- Temperature and light are the safest core sensors.
- Motion is a possible extra only if hardware is actually available.
- The dashboard should be central to the user experience.
- WebSockets are allowed, but they are optional; modern UX matters more than protocol novelty.

## 22. Immediate Next Step

Translate this PRD into:

1. final project naming
2. fixed hardware list
3. API contract
4. repo structure
5. implementation task breakdown
