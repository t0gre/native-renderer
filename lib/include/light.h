#ifndef LIGHT_H
#define LIGHT_H

#include "vec.h"

using namespace mym;

typedef struct PointLight {
  Vec3 color;
  Vec3 position;
  float constant;
  float linear;
  float quadratic;
} PointLight;

typedef struct DirectionalLight {
  Vec3 color;
  Vec3 direction;
} DirectionalLight;

typedef struct AmbientLight {
  Vec3 color;
} AmbientLight;


#endif //LIGHT_H