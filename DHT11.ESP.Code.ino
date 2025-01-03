#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

// Wi-Fi credentials
const char* ssid = "First";
const char* password = "Admin@123";

// Server URL
const char* serverURL = "http://192.168.1.25:5000/post/sensor-data";

// DHT Sensor settings
#define DHTPIN D5  // Pin where the DHT sensor is connected
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  dht.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{";
    jsonPayload += "\"temperature\": " + String(temperature) + ",";
    jsonPayload += "\"humidity\": " + String(humidity);
    jsonPayload += "}";

    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response Code: ");
      Serial.println(httpResponseCode);
      Serial.println("Response: " + http.getString());
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  } else {
    Serial.println("Wi-Fi Disconnected");
  }

  delay(10000); // Delay for 10 seconds
}
