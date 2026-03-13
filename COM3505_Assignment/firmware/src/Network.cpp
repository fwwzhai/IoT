// Network.cpp
// COM3505 assignment network module
//
// This file manages Wi-Fi connection status and provides the placeholder
// structure for future communication with the Flask backend.

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

// ---------------------------------------------------------------------------
// beginWifiConnection()
// Start a station-mode connection attempt using the configured credentials.
// ---------------------------------------------------------------------------

void beginWifiConnection() {
  WiFi.begin(Secrets::WIFI_SSID, Secrets::WIFI_PASSWORD);
}

// ---------------------------------------------------------------------------
// canUseRealWifi()
// Helper for deciding whether we should even attempt a network connection.
// ---------------------------------------------------------------------------

bool canUseRealWifi() {
  return Secrets::kHasRealWifiCredentials;
}
}

// ---------------------------------------------------------------------------
// printWifiStatus()
// Dump the most useful Wi-Fi status details to the serial monitor.
// ---------------------------------------------------------------------------

void printWifiStatus() {
  dbg(netDBG, "Wi-Fi status = ");
  dln(netDBG, static_cast<int>(WiFi.status()));

  if (WiFi.status() == WL_CONNECTED) {
    dbg(netDBG, "local IP = ");
    dln(netDBG, WiFi.localIP());
  }
}

// ---------------------------------------------------------------------------
// setupNetwork()
// Configure station mode and optionally begin Wi-Fi connection.
// ---------------------------------------------------------------------------

void setupNetwork(DeviceState& state) {
  WiFi.mode(WIFI_STA);

  dln(startupDBG, "setupNetwork...");

  if (!canUseRealWifi()) {
    dln(netDBG, "Wi-Fi credentials not configured yet.");
    dln(netDBG, "Create firmware/include/Secrets.h from Secrets.example.h.");
    state.wifiConnected = false;
    state.serverReachable = false;
    return;
  }

  dbg(netDBG, "target server = ");
  dln(netDBG, Secrets::SERVER_BASE_URL);
  dbg(netDBG, "connecting to Wi-Fi SSID = ");
  dln(netDBG, Secrets::WIFI_SSID);

  beginWifiConnection();
  g_lastConnectAttemptMs = millis();
}

// ---------------------------------------------------------------------------
// updateNetwork()
// Track Wi-Fi status and prepare for regular server synchronisation.
// ---------------------------------------------------------------------------

void updateNetwork(DeviceState& state, unsigned long now) {
  const wl_status_t currentStatus = WiFi.status();
  state.wifiConnected = currentStatus == WL_CONNECTED;

  if (currentStatus != g_lastWifiStatus) {
    g_lastWifiStatus = currentStatus;

    if (state.wifiConnected) {
      dln(netDBG, "Wi-Fi connected.");
      printWifiStatus();
    } else {
      dbg(netDBG, "Wi-Fi status changed: ");
      dln(netDBG, static_cast<int>(currentStatus));
    }
  }

  if (!canUseRealWifi()) {
    state.serverReachable = false;
    return;
  }

  if (!state.wifiConnected) {
    state.serverReachable = false;

    if (now - g_lastConnectAttemptMs >= Config::kNetworkReconnectIntervalMs) {
      dln(netDBG, "Retrying Wi-Fi connection...");
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
  dbg(netDBG, "network sync tick to ");
  dln(netDBG, Secrets::SERVER_BASE_URL);
  state.serverReachable = true;
}
