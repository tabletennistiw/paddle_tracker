#include "server.h"
#include "sensor.h"
#include <WebServer.h>
#include <LittleFS.h>

// Helper to serve any file from LittleFS
static void serveFile(WebServer& server, const char* path, const char* mime) {
  server.on(path, HTTP_GET, [&server, path, mime]() {
    File f = LittleFS.open(path, "r");
    if (!f) {
      server.send(404, "text/plain", String(path) + " not found in LittleFS");
      return;
    }
    server.streamFile(f, mime);
    f.close();
  });
}

void serverSetup(WebServer& server) {
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed!");
    return;
  }

  // Print LittleFS contents
  Serial.println("LittleFS contents:");
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  while (file) {
    Serial.printf("  %s (%d bytes)\n", file.name(), file.size());
    file = root.openNextFile();
  }

  // Serve index.html with relaxed CSP
  server.on("/", HTTP_GET, [&server]() {
    File f = LittleFS.open("/index.html", "r");
    if (!f) { server.send(404, "text/plain", "index.html not found"); return; }
    server.sendHeader("Content-Security-Policy",
      "default-src 'self' https://cdnjs.cloudflare.com https://fonts.googleapis.com https://fonts.gstatic.com; "
      "script-src 'self' 'unsafe-eval' https://cdnjs.cloudflare.com; "
      "style-src 'self' 'unsafe-inline' https://fonts.googleapis.com; "
      "font-src https://fonts.gstatic.com");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.streamFile(f, "text/html");
    f.close();
  });

  server.on("/index.html", HTTP_GET, [&server]() {
    File f = LittleFS.open("/index.html", "r");
    if (!f) { server.send(404, "text/plain", "not found"); return; }
    server.sendHeader("Content-Security-Policy",
      "default-src 'self' https://cdnjs.cloudflare.com https://fonts.googleapis.com https://fonts.gstatic.com; "
      "script-src 'self' 'unsafe-eval' https://cdnjs.cloudflare.com; "
      "style-src 'self' 'unsafe-inline' https://fonts.googleapis.com; "
      "font-src https://fonts.gstatic.com");
    server.streamFile(f, "text/html");
    f.close();
  });

  // JS files
  serveFile(server, "/viz.js",  "application/javascript");
  serveFile(server, "/ui.js",   "application/javascript");
  serveFile(server, "/data.js", "application/javascript");

  // Sensor data endpoint
  server.on("/data", HTTP_GET, [&server]() {
    SensorData d = sensorGet();
    String json = "{";
    json += "\"pitch\":"  + String(d.pitch, 2) + ",";
    json += "\"roll\":"   + String(d.roll,  2) + ",";
    json += "\"yaw\":"    + String(d.yaw,   2) + ",";
    json += "\"ax\":"     + String(d.ax,    3) + ",";
    json += "\"ay\":"     + String(d.ay,    3) + ",";
    json += "\"az\":"     + String(d.az,    3);
    json += "}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
  });

  server.on("/favicon.ico", HTTP_GET, [&server]() {
    server.send(204);
  });

  server.onNotFound([&server]() {
    Serial.println("404: " + server.uri());
    server.send(404, "text/plain", "Not found: " + server.uri());
  });

  server.begin();
}