#pragma once

struct SensorData {
  float pitch, roll, yaw;
  float ax, ay, az;
  float temp;
};

void sensorSetup();
void sensorUpdate();
SensorData sensorGet();