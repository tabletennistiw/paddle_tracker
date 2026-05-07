#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA, SCL
  delay(100);

  Serial.println("Initializing MPU-6050...");
  mpu.initialize();

  if (mpu.testConnection()) {
    Serial.println("MPU-6050 connected successfully!");
  } else {
    Serial.println("MPU-6050 connection failed. Check wiring.");
    while (1); // halt
  }

  // Optional: configure ranges
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);  // ±2g
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);  // ±250 °/s
}

void loop() {
  // Raw sensor values
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  int16_t temp_raw;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  temp_raw = mpu.getTemperature();

  // --- Accelerometer (raw → g) ---
  // At ±2g range: 1g = 16384 LSB
  float accel_x = ax / 16384.0;
  float accel_y = ay / 16384.0;
  float accel_z = az / 16384.0;

  // --- Gyroscope (raw → °/s) ---
  // At ±250°/s range: 1°/s = 131 LSB
  float gyro_x = gx / 131.0;
  float gyro_y = gy / 131.0;
  float gyro_z = gz / 131.0;

  // --- Temperature (raw → °C) ---
  float temp_c = (temp_raw / 340.0) + 36.53;

  // --- Tilt angles from accelerometer (degrees) ---
  float roll  = atan2(accel_y, accel_z) * 180.0 / PI;
  float pitch = atan2(-accel_x, sqrt(accel_y * accel_y + accel_z * accel_z)) * 180.0 / PI;

  // --- Print all readings ---
  Serial.println("=== MPU-6050 Readings ===");

  Serial.print("Accel (g)   X: "); Serial.print(accel_x, 3);
  Serial.print("  Y: ");           Serial.print(accel_y, 3);
  Serial.print("  Z: ");           Serial.println(accel_z, 3);

  Serial.print("Gyro (°/s)  X: "); Serial.print(gyro_x, 2);
  Serial.print("  Y: ");           Serial.print(gyro_y, 2);
  Serial.print("  Z: ");           Serial.println(gyro_z, 2);

  Serial.print("Tilt        Roll: ");  Serial.print(roll, 1);
  Serial.print("°  Pitch: ");         Serial.print(pitch, 1);
  Serial.println("°");

  Serial.print("Temperature: "); Serial.print(temp_c, 1);
  Serial.println(" °C");

  Serial.println();
  delay(500);
}