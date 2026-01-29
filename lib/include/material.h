#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec.h"
#include <variant>
#include "mystl.hpp"
#include <string>
#include <GLES3/gl3.h>



struct BasicColorMaterial {
      Vec3 color;
      Vec3 specular_color;
      float shininess;
};

struct BasicTextureMaterial
{
      DArray<float> texture;
      DArray<float> uvMap;
      std::string texture_path; // path reported by Assimp (may be embedded like "*0")
      GLuint texture_id; // OpenGL texture ID once loaded
      float shininess;
};

using Material = std::variant<BasicColorMaterial, BasicTextureMaterial>;

#endif