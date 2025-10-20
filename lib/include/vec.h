#ifndef VEC_H
#define VEC_H
#include <math.h>
#include <stdbool.h>
typedef union Vec2 { 
  struct {
    float x;
    float y;
  };

  float data [2];
} Vec2;

typedef union Vec3 {
  struct { 
    float x;
    float y;
    float z;
  };
  struct { 
    float r;
    float g;
    float b;
  };

  float data[3];

} Vec3;

typedef union Vec4 {
  struct {
    float x;
    float y;
    float z;
    float w;
  };

  float data[4];

} Vec4;

// result type for vec3
typedef struct {
    bool valid;   
    Vec3 value; 
} Vec3Result;

Vec3 scaleVector(Vec3 vec, float scalar);

Vec3 addVectors(Vec3 a, Vec3 b);

Vec3 subtractVectors(Vec3 a, Vec3 b);

Vec3 normalize(Vec3 v);

Vec3 cross(Vec3 a, Vec3 b);

float dot(Vec3 a, Vec3 b);

float length(Vec3 v);

Vec3 calculateOrbitPosition(
    float azimuth, 
    float elevation, 
    Vec3 orbitTarget,
    float orbitRadius
);

#endif //VEC_H 