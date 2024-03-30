#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Moto G60";
const char* password = "vkab6125";
const char* serverAddress = "http://127.0.0.1:5000";
const int serverPort = 80;

const int soilMoisturePin = A0; 
const int pumpPin = D1;         

void setup() {
  Serial.begin(9600);
  pinMode(soilMoisturePin, INPUT);
  pinMode(pumpPin, OUTPUT);
  connectToWiFi();
}

void loop() {
  int moistureLevel = analogRead(soilMoisturePin);
  Serial.print("Soil Moisture Level: ");
  Serial.println(moistureLevel);

  sendSoilMoistureToServer(moistureLevel);

  delay(10000);
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");
}

void sendSoilMoistureToServer(int moistureLevel) {
  WiFiClient client;
  HTTPClient http;

  String url = "/api/soil-moisture?level=" + String(moistureLevel);

  Serial.print("Sending API request to server: ");
  Serial.println(url);

  if (http.begin(client, serverAddress, serverPort, url)) {
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      String response = http.getString();
      Serial.print("Server response: ");
      Serial.println(response);

      if (response.toInt() == 1) {
        digitalWrite(pumpPin, HIGH);
        Serial.println("Pump turned ON");
      } else {
        digitalWrite(pumpPin, LOW); 
        Serial.println("Pump turned OFF");
      }
    } else {
      Serial.print("Error sending API request: ");
      Serial.println(http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.println("Unable to connect to server");
  }
}
