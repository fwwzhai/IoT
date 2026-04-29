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
#include <WiFiClient.h>

namespace {
unsigned long g_lastConnectAttemptMs = 0;
unsigned long g_lastSensorUploadMs = 0;
unsigned long g_lastControlPollMs = 0;
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

// ---------------------------------------------------------------------------
// buildSensorPayload()
// Serialize the most important device and sensor state into a compact JSON
// string using core Arduino String operations.
// ---------------------------------------------------------------------------

String buildSensorPayload(const DeviceState& state) {
  String payload = "{";

  payload += "\"device\":{";
  payload += "\"wifi_connected\":";
  payload += state.wifiConnected ? "true" : "false";
  payload += "},";

  payload += "\"sensors\":{";
  payload += "\"temperature_c\":";
  payload += String(state.sensors.temperatureC, 1);
  payload += ",\"button_pressed\":";
  payload += state.sensors.buttonPressed ? "true" : "false";
  payload += "}";

  payload += "}";
  return payload;
}

String buildModePayload(DeviceMode mode) {
  String payload = "{\"mode\":\"";
  payload += modeName(mode);
  payload += "\"}";
  return payload;
}

String buildPatternPayload(PatternId pattern) {
  String payload = "{\"pattern\":\"";
  payload += patternName(pattern);
  payload += "\"}";
  return payload;
}

// ---------------------------------------------------------------------------
// readHttpResponse()
// Read the full response from a connected WiFiClient until the connection
// closes or a timeout is reached.
// ---------------------------------------------------------------------------

String readHttpResponse(WiFiClient& client) {
  String response;
  unsigned long lastDataMs = millis();

  while (millis() - lastDataMs < Config::kSocketTimeoutMs) {
    while (client.available()) {
      response += static_cast<char>(client.read());
      lastDataMs = millis();
    }

    if (!client.connected() && !client.available()) {
      break;
    }

    delay(1);
  }

  client.stop();
  return response;
}

// ---------------------------------------------------------------------------
// parseStatusCode()
// Extract the HTTP status code from the status line.
// ---------------------------------------------------------------------------

int parseStatusCode(const String& response) {
  const int firstSpace = response.indexOf(' ');
  if (firstSpace < 0) {
    return -1;
  }

  const int secondSpace = response.indexOf(' ', firstSpace + 1);
  if (secondSpace < 0) {
    return -1;
  }

  return response.substring(firstSpace + 1, secondSpace).toInt();
}

// ---------------------------------------------------------------------------
// extractResponseBody()
// Split headers from body using the standard HTTP blank line separator.
// ---------------------------------------------------------------------------

String extractResponseBody(const String& response) {
  int bodyStart = response.indexOf("\r\n\r\n");
  if (bodyStart >= 0) {
    return response.substring(bodyStart + 4);
  }

  bodyStart = response.indexOf("\n\n");
  if (bodyStart >= 0) {
    return response.substring(bodyStart + 2);
  }

  return "";
}

// ---------------------------------------------------------------------------
// extractJsonString()
// Lightweight JSON field extraction for simple quoted string values.
// ---------------------------------------------------------------------------

String extractJsonString(const String& json, const char* key) {
  const String quotedKey = String("\"") + key + "\"";
  const int keyPos = json.indexOf(quotedKey);
  if (keyPos < 0) {
    return "";
  }

  const int colonPos = json.indexOf(':', keyPos + quotedKey.length());
  if (colonPos < 0) {
    return "";
  }

  const int firstQuote = json.indexOf('"', colonPos + 1);
  if (firstQuote < 0) {
    return "";
  }

  const int secondQuote = json.indexOf('"', firstQuote + 1);
  if (secondQuote < 0) {
    return "";
  }

  return json.substring(firstQuote + 1, secondQuote);
}

// ---------------------------------------------------------------------------
// modeFromString() / patternFromString()
// Convert JSON string values into internal enums.
// ---------------------------------------------------------------------------

DeviceMode modeFromString(const String& mode) {
  return mode == "auto" ? DeviceMode::Auto : DeviceMode::Manual;
}

PatternId patternFromString(const String& pattern) {
  if (pattern == "chase") {
    return PatternId::Chase;
  }

  if (pattern == "cycle") {
    return PatternId::Cycle;
  }

  if (pattern == "alert") {
    return PatternId::Alert;
  }

  if (pattern == "pulse") {
    return PatternId::Pulse;
  }

  if (pattern == "heartbeat") {
    return PatternId::Heartbeat;
  }

  return PatternId::Blink;
}

// ---------------------------------------------------------------------------
// sendRequest()
// Shared HTTP helper used for both GET and POST requests.
// ---------------------------------------------------------------------------

int sendRequest(
  const char* method,
  const char* path,
  const String& body,
  String& responseBody
) {
  WiFiClient client;

  if (!client.connect(Secrets::SERVER_HOST, Secrets::SERVER_PORT)) {
    dln(netDBG, "server connection failed");
    responseBody = "";
    return -1;
  }

  client.print(String(method) + " " + path + " HTTP/1.1\r\n");
  client.print(String("Host: ") + Secrets::SERVER_HOST + "\r\n");
  client.print("Connection: close\r\n");

  if (String(method) == "POST") {
    client.print("Content-Type: application/json\r\n");
    client.print(String("Content-Length: ") + body.length() + "\r\n");
  }

  client.print("\r\n");

  if (String(method) == "POST") {
    client.print(body);
  }

  const String rawResponse = readHttpResponse(client);
  responseBody = extractResponseBody(rawResponse);
  return parseStatusCode(rawResponse);
}

// ---------------------------------------------------------------------------
// syncSensorState()
// Push the current device and sensor state to Flask.
// ---------------------------------------------------------------------------

bool syncSensorState(const DeviceState& state) {
  String responseBody;
  const String payload = buildSensorPayload(state);
  const int statusCode = sendRequest("POST", "/api/sensor", payload, responseBody);

  dbg(netDBG, "sensor POST status = ");
  dln(netDBG, statusCode);

  return statusCode == 200;
}

bool syncLocalControlState(const DeviceState& state) {
  String responseBody;

  const int modeStatus = sendRequest(
    "POST",
    "/api/mode",
    buildModePayload(state.mode),
    responseBody
  );
  dbg(netDBG, "mode POST status = ");
  dln(netDBG, modeStatus);

  const int patternStatus = sendRequest(
    "POST",
    "/api/pattern",
    buildPatternPayload(state.pattern),
    responseBody
  );
  dbg(netDBG, "pattern POST status = ");
  dln(netDBG, patternStatus);

  return modeStatus == 200 && patternStatus == 200;
}

// ---------------------------------------------------------------------------
// fetchControlState()
// Pull the latest mode and pattern selection from Flask.
// ---------------------------------------------------------------------------

bool fetchControlState(DeviceState& state) {
  String responseBody;
  const int statusCode = sendRequest("GET", "/api/state", "", responseBody);

  dbg(netDBG, "state GET status = ");
  dln(netDBG, statusCode);

  if (statusCode != 200) {
    return false;
  }

  const String mode = extractJsonString(responseBody, "mode");
  const String pattern = extractJsonString(responseBody, "pattern");

  if (mode.length() > 0) {
    state.mode = modeFromString(mode);
  }

  if (pattern.length() > 0) {
    state.pattern = patternFromString(pattern);
  }

  return true;
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
  dbg(netDBG, "server host = ");
  dln(netDBG, Secrets::SERVER_HOST);
  dbg(netDBG, "server port = ");
  dln(netDBG, Secrets::SERVER_PORT);
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

  bool requestAttempted = false;
  bool requestSucceeded = false;

  if (state.localControlDirty) {
    requestAttempted = true;
    requestSucceeded = syncLocalControlState(state) || requestSucceeded;

    if (requestSucceeded) {
      state.localControlDirty = false;
    } else {
      state.serverReachable = false;
      return;
    }
  }

  if (now - g_lastControlPollMs >= Config::kControlPollIntervalMs) {
    g_lastControlPollMs = now;
    requestAttempted = true;
    requestSucceeded = fetchControlState(state) || requestSucceeded;
  }

  if (now - g_lastSensorUploadMs >= Config::kSensorUploadIntervalMs) {
    g_lastSensorUploadMs = now;
    requestAttempted = true;
    requestSucceeded = syncSensorState(state) || requestSucceeded;
  }

  if (requestAttempted) {
    state.serverReachable = requestSucceeded;
  }
}
