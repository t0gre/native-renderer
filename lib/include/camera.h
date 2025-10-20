#ifndef CAMERA_H
#define CAMERA_H

#include "vec.h"
#include "mat4.h"

typedef struct Orbit {
  float azimuth;
  float elevation;
  float sensitivity;
  float radius;
  Vec3 target;
} Orbit;


typedef struct Camera {
  float field_of_view_radians;
  float aspect;
  float near;
  float far;
  Vec3 up;
  Mat4 transform;
  Orbit orbit;
} Camera;

Mat4 getProjectionMatrix(Camera camera);

Mat4 getViewMatrix(Camera camera);


#endif //CAMERA_H