# COM3505 Assignment

ESP32-S3 temperature monitoring system with a TMP36 sensor, LED pattern output,
Flask backend, and AJAX-updated web dashboard.

## Contents

- `firmware/` ESP32 PlatformIO project
- `server/` Flask backend and dashboard
- `docs/` supporting project files
- `Dockerfile` Flask dashboard container

## Run Dashboard

```powershell
cd COM3505_Assignment
python -m pip install -r server/requirements.txt
python server/app.py
```

Open:

```text
http://127.0.0.1:5000
```

Docker alternative:

```powershell
cd COM3505_Assignment
docker build -t com3505-dashboard .
docker run --rm -p 5000:5000 com3505-dashboard
```

## Build Firmware

```powershell
cd COM3505_Assignment\firmware
pio run
pio run --target upload
pio device monitor --baud 115200
```

Network settings are loaded from `firmware/include/Secrets.h`.
Use `firmware/include/Secrets.example.h` as the local template.

## Network Settings

The ESP32 and Flask server must be on the same Wi-Fi network.

`firmware/include/Secrets.h` defines:

```cpp
namespace Secrets {
constexpr bool kHasRealWifiCredentials = true;
constexpr char WIFI_SSID[] = "WIFI_NAME";
constexpr char WIFI_PASSWORD[] = "WIFI_PASSWORD";
constexpr char SERVER_HOST[] = "192.168.0.89";
constexpr uint16_t SERVER_PORT = 5000;
constexpr char SERVER_BASE_URL[] = "http://192.168.0.89:5000";
}
```

`SERVER_HOST` is the LAN IPv4 address printed by Flask when `python server/app.py`
starts. Example:

```text
Running on http://192.168.0.89:5000
```

## Pin Map

| Function | Pin |
| --- | --- |
| TMP36 signal | `A0` |
| Button input | `GPIO5` |
| Red LED | `GPIO6` |
| Yellow LED | `GPIO9` |
| Green LED | `GPIO12` |
