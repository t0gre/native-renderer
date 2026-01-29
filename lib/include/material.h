#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec.h"
#include <variant>
#include "mystl.hpp"



struct BasicColorMaterial {
      Vec3 color;
      Vec3 specular_color;
      float shininess;
};

struct BasicTextureMaterial
{
      DArray<float> texture;
      DArray<float> uvMap;
      float shininess;
};

using Material = std::variant<BasicColorMaterial, BasicTextureMaterial>;

#endif