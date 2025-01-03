// Soil Mositure Sensor
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "First";
const char* password = "Admin@123";

const char* serverURL = "http://localhost:5000/post/sensor-data";
const int SoilPin = A0;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
   float soilPer = map(analogRead(SoilPin,1024,0,0,100));
   
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{";
    jsonPayload += "\"soil_value\": " + String(soilPer);
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
