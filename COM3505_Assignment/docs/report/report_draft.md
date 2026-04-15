# COM3505 Internet of Things Assignment Report Draft

## Title

Smart Ambient Monitoring Node: ESP32 Sensor and LED Control via Flask Web Interface

## 1. Introduction

This project implements an Internet of Things system using an ESP32 Feather board, a TMP36 temperature sensor, and three LEDs controlled through a Python Flask web interface. The aim of the project was to satisfy the assignment requirements for sensing, actuation, networking, and browser-based monitoring while producing a system that is stable, modular, and easy to explain.

The final system reads sensor data from the ESP32, connects to a local Wi-Fi network, sends telemetry to a Flask server, and displays live information in a browser dashboard. The same dashboard also allows the user to change LED patterns and switch between manual and automatic control modes.

## 2. Hardware Setup

The hardware used in the current build consists of:

- Adafruit Feather ESP32-S3
- TMP36 temperature sensor
- 3 LEDs
  - red
  - yellow
  - green
- 3 current-limiting resistors
- Breadboard and jumper wires

The TMP36 sensor output is connected to analog pin `A0` on the ESP32. The three LEDs are connected to GPIO pins `6`, `9`, and `12`, matching the firmware pin map in [Pins.h](C:/University%20of%20Sheffield/Notes/Internet%20Of%20Things/IoT/COM3505_Assignment/firmware/include/Pins.h). Each LED is connected in series with a resistor to protect the LED and the board.

Insert hardware diagram here.

Suggested caption:

Hardware wiring for the ESP32 ambient monitoring node, showing the TMP36 temperature sensor connected to `A0` and three LEDs connected to GPIO `6`, `9`, and `12`.

## 3. System Architecture

The system is divided into three main parts:

- ESP32 firmware
- Flask backend
- browser dashboard

The ESP32 is responsible for reading sensors, generating LED patterns, and sending data over Wi-Fi. The Flask server acts as the coordination layer between the hardware and the browser. The browser dashboard displays live values and sends control commands back to the server, which are then read by the ESP32.

High-level data flow:

1. The ESP32 samples the temperature sensor.
2. The ESP32 updates the LED pattern engine.
3. The ESP32 sends the latest device state and sensor values to Flask using HTTP and JSON.
4. Flask stores the latest readings and current mode/pattern state.
5. The dashboard polls the server using JavaScript and updates the page without refresh.
6. Pattern and mode changes made in the browser are sent back to Flask and then fetched by the ESP32.

Insert architecture diagram here.

Suggested caption:

System architecture showing sensor input, LED control, Wi-Fi communication, Flask APIs, and browser-based monitoring.

## 4. Firmware Design

The firmware is structured as a modular PlatformIO project rather than a single Arduino sketch. This makes the code easier to understand and supports cleaner separation of responsibilities.

The main firmware modules are:

- [main.cpp](C:/University%20of%20Sheffield/Notes/Internet%20Of%20Things/IoT/COM3505_Assignment/firmware/src/main.cpp): Arduino entry point
- [Thing.cpp](C:/University%20of%20Sheffield/Notes/Internet%20Of%20Things/IoT/COM3505_Assignment/firmware/src/Thing.cpp): top-level orchestration and shared state
- [Sensors.cpp](C:/University%20of%20Sheffield/Notes/Internet%20Of%20Things/IoT/COM3505_Assignment/firmware/src/Sensors.cpp): sensor sampling
- [Patterns.cpp](C:/University%20of%20Sheffield/Notes/Internet%20Of%20Things/IoT/COM3505_Assignment/firmware/src/Patterns.cpp): LED pattern engine
- [Network.cpp](C:/University%20of%20Sheffield/Notes/Internet%20Of%20Things/IoT/COM3505_Assignment/firmware/src/Network.cpp): Wi-Fi and HTTP communication

This design avoids putting unrelated logic into a single `loop()` function. Instead, the system maintains a shared `DeviceState` structure and updates sensing, animation, and networking separately.

### Sensor Reading

The current build uses a TMP36 temperature sensor. The sensor value is read using the ESP32 ADC and converted into degrees Celsius. Sampling is scheduled using `millis()` so the firmware does not block during operation.

### LED Pattern Engine

The system controls three LEDs using a logical LED buffer with red, yellow, and green channels. Multiple dynamic patterns are implemented, including blink, chase, cycle, alert, pulse, and heartbeat. The active pattern is updated over time using non-blocking timing.

### Timing Strategy

The firmware uses `millis()`-based scheduling instead of relying on large blocking delays. This is important because the device must handle sensor reading, LED animation, and server communication at the same time. Using separate timers improves responsiveness and helps the system remain stable.

## 5. Wi-Fi and Server Communication

The ESP32 connects to the local Wi-Fi network in station mode. Once connected, the firmware prints the IP address to the serial monitor for debugging and troubleshooting. The firmware then communicates with the Flask server using HTTP requests carrying JSON payloads.

The main API interactions are:

- `POST /api/sensor`
  - uploads current sensor values and device state
- `GET /api/state`
  - fetches the latest mode and pattern

This design keeps the ESP32 side relatively simple while still supporting live interaction with the browser interface.

## 6. Flask Server and Web Interface

The Flask server stores the latest sensor values, the active LED pattern, the current mode, and recent history for dashboard display. It also serves the dashboard web page and exposes API routes for both the ESP32 and the browser.

The dashboard provides:

- live temperature display
- device health status
- current mode and pattern display
- pattern selection buttons
- mode switching between manual and auto
- live history chart

The browser uses JavaScript polling to request updated state and history from the Flask server. This means the page updates automatically without requiring a manual refresh. This satisfies the assignment requirement for live web updates and matches the acceptable AJAX approach described in the brief.

## 7. Automatic Behaviour and Extra Features

In addition to the core assignment requirements, the system includes features aimed at a higher-mark submission:

- manual and automatic operating modes
- multiple LED animation patterns
- recent sensor history chart
- responsive dashboard layout
- health indicator showing whether the device is live, stale, or offline

In automatic mode, the firmware can change the LED pattern based on sensor thresholds. This improves the practical usefulness of the system by allowing it to react to environmental conditions instead of only following manual commands.

## 8. Testing and Results

The system was tested by running the Flask server locally, connecting the ESP32 to Wi-Fi, and opening the dashboard in a browser. Successful operation was verified by checking:

- serial monitor output showing Wi-Fi connection and IP address
- periodic sensor updates from the ESP32
- live dashboard updates without page refresh
- visible LED pattern changes
- successful browser control of pattern and mode

The final tests showed that the device could connect to Wi-Fi, upload temperature data to Flask, and update the dashboard while continuing to animate the LEDs correctly.

## 9. Evaluation

The project meets the main functional requirements of the assignment:

- sensor integration
- three-LED dynamic control
- Wi-Fi connection
- communication with a Python Flask server
- browser-based live monitoring
- browser-based LED control

A major strength of the project is the separation between sensing, animation, networking, and presentation. This makes the code easier to debug and improves report clarity. Another strength is the end-to-end integration between hardware, backend, and dashboard.

One limitation of the current bring-up configuration is that the firmware is still using a TMP36-focused test mode, with some optional sensors stubbed until final hardware validation. However, this does not prevent the system from meeting the core assignment requirements because the brief requires at least one working sensor.

## 10. Conclusion

This project demonstrates a complete ESP32-based IoT system that combines sensing, LED control, Wi-Fi communication, a Flask backend, and a live browser dashboard. The system satisfies the core assignment brief and includes additional features such as auto mode, a live graph, and a responsive interface. The final result is not just a basic prototype, but a coherent and well-structured IoT application that is suitable for demonstration and discussion in the report.

## Final Checklist Before Submission

- Insert hardware diagram
- Insert system architecture diagram
- Add one screenshot of the dashboard
- Trim wording to fit within the 4-page limit
- Check all figure captions
- Check that `Secrets.h` is not submitted
- Record the short demo video
