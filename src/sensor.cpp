#include "sensor.h"
#include "kalman.h"
#include <MPU6050.h>
#include <Wire.h>
#include <math.h>

static MPU6050 mpu;
static Kalman kalmanPitch, kalmanRoll;
static SensorData data;
static unsigned long lastTime = 0;

float Kalman::update(float newAngle, float newRate, float dt) {
  float rate = newRate - bias;
  angle += dt * rate;
  P[0][0] += dt * (dt * P[1][1] - P[0][1] - P[1][0] + Q_angle);
  P[0][1] -= dt * P[1][1];
  P[1][0] -= dt * P[1][1];
  P[1][1] += Q_bias * dt;
  float S  = P[0][0] + R_measure;
  float K0 = P[0][0] / S;
  float K1 = P[1][0] / S;
  float y  = newAngle - angle;
  angle += K0 * y;
  bias  += K1 * y;
  P[0][0] -= K0 * P[0][0];
  P[0][1] -= K0 * P[0][1];
  P[1][0] -= K1 * P[0][0];
  P[1][1] -= K1 * P[0][1];
  return angle;
}

void sensorSetup() {
  Wire.begin(21, 22);
  delay(100);
  mpu.initialize();
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float ax0 = ax / 16384.0f, ay0 = ay / 16384.0f, az0 = az / 16384.0f;
  kalmanPitch.angle = atan2(-ax0, sqrt(ay0*ay0 + az0*az0)) * 180.0f / PI;
  kalmanRoll.angle  = atan2(ay0, az0) * 180.0f / PI;
  lastTime = micros();
}

void sensorUpdate() {
  unsigned long now = micros();
  float dt = (now - lastTime) / 1000000.0f;
  lastTime = now;

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  data.ax = ax / 16384.0f;
  data.ay = ay / 16384.0f;
  data.az = az / 16384.0f;

  float gyro_x = gx / 131.0f;
  float gyro_y = gy / 131.0f;
  float gyro_z = gz / 131.0f;

  float accel_pitch = atan2(-data.ax, sqrt(data.ay*data.ay + data.az*data.az)) * 180.0f / PI;
  float accel_roll  = atan2(data.ay, data.az) * 180.0f / PI;

  data.pitch  = kalmanPitch.update(accel_pitch, gyro_y, dt);
  data.roll   = kalmanRoll.update(accel_roll,   gyro_x, dt);
  data.yaw   += gyro_z * dt;
}

SensorData sensorGet() { return data; }