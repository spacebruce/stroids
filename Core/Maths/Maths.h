#pragma once

// Constant Macros
#define PI 3.14
#define DEGREE_FACTOR (57.295779) // full: 57.295779513082320876798154814105
#define RADIAN_FACTOR (0.017453) // full: 0.01745329251994329576923690768489

// Function Macros
#define ToDegrees(x) ((x) * (DEGREE_FACTOR))
#define ToRadians(x) ((x) * (RADIAN_FACTOR))
