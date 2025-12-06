#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "OnePlus Nord 3 5G";
const char* password = "ChocolateTruffle";

const char* pulseURL = "http://192.168.10.97:5000/pulse";
const char* ledControlURL = "http://192.168.10.97:5000/led_control";

const int pulsePin = 34;
const int ledPin = 2;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  Serial.println("WiFi Connected");
}

void loop() {
  int pulseValue = analogRead(pulsePin);
  Serial.print("Pulse: ");
  Serial.println(pulseValue);

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    http.begin(pulseURL);
    http.addHeader("Content-Type", "application/json");

    String pulsePayload = "{\"pulse_value\": " + String(pulseValue) + "}";
    http.POST(pulsePayload);
    http.end();

    delay(300);

    http.begin(ledControlURL);
    int response = http.GET();

    if (response > 0) {
      String body = http.getString();
      StaticJsonDocument<200> doc;
      deserializeJson(doc, body);

      const char* ledStatus = doc["led_status"];
      if (strcmp(ledStatus, "ON") == 0) {
        digitalWrite(ledPin, HIGH);
      } else {
        digitalWrite(ledPin, LOW);
      }
    }

    http.end();
  }

  delay(1000);
}
