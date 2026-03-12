// Ex11 – NTC Thermistor Temperature + Light Sensor + PWM LED + Web Dashboard

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <math.h>

// WiFi credentials
const char* ssid = "VM7572372";
const char* password = "9qybfyKBgsnc"; // CHANGE THIS TO YOUR WIFI PASSWORD

// Pins
const int thermistorPin = 34;   // NTC voltage divider output
const int lightPin = 35;        // LDR voltage divider output
const int ledPin = 2;           // LED PWM output

// Thermistor constants
const float referenceVoltage = 3.3;
const float referenceResistor = 10000.0;   // 10k resistor
const float beta = 3950.0;
const float nominalTemperature = 25.0;
const float nominalResistance = 10000.0;

// Web server
WebServer server(80);

// ADC
#define ADC_MAX 4095.0

// PWM channel
const int pwmChannel = 0;

float temperatureC = 0;
int lightValue = 0;

void handleRoot()
{
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>ESP32 Sensor Dashboard</title>";
  html += "<meta http-equiv='refresh' content='2'>";
  html += "</head><body>";

  html += "<h2>COM3505 IoT Exercise 12 ESP32 Sensors</h2>";

  html += "<p><b>Temperature:</b> ";
  html += String(temperatureC,1);
  html += " &deg;C</p>";

  html += "<p><b>Light Level:</b> ";
  html += String(lightValue);
  html += "</p>";

  html += "</body></html>";

  server.send(200,"text/html",html);
}

void setup()
{
  Serial.begin(115200);
  delay(500);

  Serial.println("=== Ex11: NTC Thermistor + Light Sensor ===");

  // ADC configuration
  analogReadResolution(12);
  analogSetPinAttenuation(thermistorPin, ADC_11db);
  analogSetPinAttenuation(lightPin, ADC_11db);

  // PWM setup
  ledcSetup(pwmChannel, 5000, 8);
  ledcAttachPin(ledPin, pwmChannel);

  // WiFi
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Web server
  server.on("/", handleRoot);
  server.begin();
}

void loop()
{
  server.handleClient();

  // Read thermistor ADC
  int adcValue = analogRead(thermistorPin);

  float voltage = adcValue * referenceVoltage / ADC_MAX;

  // Voltage divider equation
  float resistance = (voltage * referenceResistor) / (referenceVoltage - voltage);

  // Beta equation
  float tempK = 1 / ((log(resistance / nominalResistance) / beta) +
                     (1.0 / (nominalTemperature + 273.15)));

  temperatureC = tempK - 273.15;

  // Read light sensor
  lightValue = analogRead(lightPin);

  // Control LED brightness (inverse mapping)
  int brightness = map(lightValue, 0, 4095, 255, 0);

  ledcWrite(pwmChannel, brightness);

  // Serial output
  Serial.printf("ADC: %d  ", adcValue);
  Serial.printf("Temp: %.2f C  ", temperatureC);
  Serial.printf("Light: %d  ", lightValue);
  Serial.printf("LED: %d\n", brightness);

  delay(1000);
}