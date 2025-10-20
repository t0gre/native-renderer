#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec.h"

typedef struct Material {
      Vec3 color;
      Vec3 specular_color;
      float shininess;
} Material;

#endif