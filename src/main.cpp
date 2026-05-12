#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "sensor.h"
#include "server.h"
#include <ESPmDNS.h>

const char* SSID = "Brownie";
const char* PASSWORD = "wiNNer2024$";

WebServer server(80);

void setup() {
  delay(5000);
  Serial.begin(115200);
  sensorSetup();

  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nOpen: http://" + WiFi.localIP().toString());

  if (MDNS.begin("paddle")) {
  Serial.println("mDNS started — open http://paddle.local");
  }

  serverSetup(server);
}

void loop() {
  sensorUpdate();
  server.handleClient();
  delay(10);
}