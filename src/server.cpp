#include "server.h"
#include "sensor.h"
#include <WebServer.h>
#include <LittleFS.h>

void serverSetup(WebServer& server) {
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed!");
    return;
  }

  server.on("/", HTTP_GET, [&]() {
    File f = LittleFS.open("/index.html", "r");
    if (!f) { server.send(404, "text/plain", "index.html not found"); return; }
    server.streamFile(f, "text/html");
    f.close();
  });

  server.on("/data", HTTP_GET, [&]() {
    SensorData d = sensorGet();
    String json = "{";
    json += "\"pitch\":"  + String(d.pitch, 2) + ",";
    json += "\"roll\":"   + String(d.roll,  2) + ",";
    json += "\"yaw\":"    + String(d.yaw,   2) + ",";
    json += "\"ax\":"     + String(d.ax,    3) + ",";
    json += "\"ay\":"     + String(d.ay,    3) + ",";
    json += "\"az\":"     + String(d.az,    3) + ",";
    json += "\"temp\":"   + String(d.temp,  1);
    json += "}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
  });

  server.begin();
}