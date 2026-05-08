#pragma once

struct Kalman {
  float angle = 0, bias = 0;
  float P[2][2] = {{0,0},{0,0}};
  const float Q_angle   = 0.001f;
  const float Q_bias    = 0.003f;
  const float R_measure = 0.03f;

  float update(float newAngle, float newRate, float dt);
};