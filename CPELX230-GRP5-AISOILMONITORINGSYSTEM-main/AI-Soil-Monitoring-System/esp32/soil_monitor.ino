/*
  AI-Based Soil Monitoring System - ESP32

  Hardware:
  - ESP32
  - Capacitive soil moisture sensor -> GPIO 34 (analog)
  - DHT22 -> GPIO 4

  Libraries required in Arduino IDE:
  - DHT sensor library by Adafruit
  - Adafruit Unified Sensor

  IMPORTANT:
  1. Replace WIFI_SSID and WIFI_PASSWORD.
  2. Replace SERVER_URL with the IP address of the PC running Flask.
     Example: http://192.168.1.5:5000/api/sensor
  3. Calibrate AIR_VALUE and WATER_VALUE for your actual soil sensor.
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHT_PIN 4
#define DHT_TYPE DHT22
#define SOIL_PIN 34

DHT dht(DHT_PIN, DHT_TYPE);

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* SERVER_URL = "http://192.168.1.5:5000/api/sensor";

// Example calibration values for a 12-bit ESP32 ADC.
// You MUST measure these using your own sensor.
const int AIR_VALUE = 3200;    // Dry / air reading
const int WATER_VALUE = 1300;  // Wet / water reading

unsigned long lastSend = 0;
const unsigned long SEND_INTERVAL = 5000;

float readSoilMoisturePercent() {
  int rawValue = analogRead(SOIL_PIN);

  float percent = 100.0 * (AIR_VALUE - rawValue) / (AIR_VALUE - WATER_VALUE);

  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;

  Serial.print("Soil raw: ");
  Serial.print(rawValue);
  Serial.print(" | Moisture: ");
  Serial.print(percent, 1);
  Serial.println("%");

  return percent;
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  analogReadResolution(12);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected. ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() - lastSend < SEND_INTERVAL) {
    return;
  }

  lastSend = millis();

  float soilMoisture = readSoilMoisturePercent();
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read DHT22.");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.print(" C | Humidity: ");
  Serial.print(humidity, 1);
  Serial.println("%");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    WiFi.reconnect();
    return;
  }

  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");

  String json =
      "{\"soil_moisture\":" + String(soilMoisture, 1) +
      ",\"temperature\":" + String(temperature, 1) +
      ",\"humidity\":" + String(humidity, 1) + "}";

  int httpCode = http.POST(json);

  Serial.print("HTTP status: ");
  Serial.println(httpCode);

  if (httpCode > 0) {
    String response = http.getString();
    Serial.print("Server response: ");
    Serial.println(response);
  } else {
    Serial.println("Failed to send data to Flask server.");
  }

  http.end();
}
