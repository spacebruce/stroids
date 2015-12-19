#pragma once

// Constant Macros
#define pi 3.14
#define degreeFactor (57.295779) // full: 57.295779513082320876798154814105
#define radianFactor (0.017453) // full: 0.01745329251994329576923690768489

// Function Macros
#define todeg(x) ((x) * (degreeFactor))
#define torad(x) ((x) * (radianFactor))
