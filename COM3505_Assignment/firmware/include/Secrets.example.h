#pragma once

// Local Secrets.h stores Wi-Fi and Flask server settings.
// The live Secrets.h file stays untracked to keep credentials private.

namespace Secrets {
constexpr bool kHasRealWifiCredentials = false;
constexpr char WIFI_SSID[] = "YOUR_WIFI_SSID";
constexpr char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";
constexpr char SERVER_HOST[] = "192.168.0.10";
constexpr uint16_t SERVER_PORT = 5000;
constexpr char SERVER_BASE_URL[] = "http://192.168.0.10:5000";
}
