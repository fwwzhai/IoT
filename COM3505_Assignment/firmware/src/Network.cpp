#include "Network.h"

#include "Config.h"

#if __has_include("Secrets.h")
#include "Secrets.h"
#else
#include "Secrets.example.h"
#endif

#include <WiFi.h>

namespace {
unsigned long g_lastConnectAttemptMs = 0;
unsigned long g_lastSyncMs = 0;
wl_status_t g_lastWifiStatus = WL_IDLE_STATUS;

void beginWifiConnection() {
  WiFi.begin(Secrets::WIFI_SSID, Secrets::WIFI_PASSWORD);
}
}

void setupNetwork(DeviceState& state) {
  WiFi.mode(WIFI_STA);

  if (!Secrets::kHasRealWifiCredentials) {
    Serial.println("Wi-Fi credentials not configured yet.");
    Serial.println("Create firmware/include/Secrets.h from Secrets.example.h.");
    state.wifiConnected = false;
    state.serverReachable = false;
    return;
  }

  Serial.printf("Target server: %s\n", Secrets::SERVER_BASE_URL);
  Serial.printf("Connecting to Wi-Fi SSID: %s\n", Secrets::WIFI_SSID);

  beginWifiConnection();
  g_lastConnectAttemptMs = millis();
}

void updateNetwork(DeviceState& state, unsigned long now) {
  const wl_status_t currentStatus = WiFi.status();
  state.wifiConnected = currentStatus == WL_CONNECTED;

  if (currentStatus != g_lastWifiStatus) {
    g_lastWifiStatus = currentStatus;

    if (state.wifiConnected) {
      Serial.print("Wi-Fi connected. IP: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.printf("Wi-Fi status changed: %d\n", currentStatus);
    }
  }

  if (!Secrets::kHasRealWifiCredentials) {
    state.serverReachable = false;
    return;
  }

  if (!state.wifiConnected) {
    state.serverReachable = false;

    if (now - g_lastConnectAttemptMs >= Config::kNetworkReconnectIntervalMs) {
      Serial.println("Retrying Wi-Fi connection...");
      beginWifiConnection();
      g_lastConnectAttemptMs = now;
    }

    return;
  }

  if (now - g_lastSyncMs < Config::kNetworkSyncIntervalMs) {
    return;
  }

  g_lastSyncMs = now;

  // Placeholder until the API contract is locked and HTTP sync is implemented.
  state.serverReachable = true;
}

