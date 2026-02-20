#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec.h"
#include <variant>
#include "mystl.hpp"
#include <string>
#include <GLES3/gl3.h>

enum class WrapMode {
      Wrap,      // Repeat/tile the texture
      Clamp,     // Clamp to edge
      Mirror,    // Mirror repeat
      Decal      // Decal/border
};

struct TextureData {
      unsigned char* pixels;  // pixel data (RGBA or RGB)
      int width;
      int height;
      int channels;           // number of color channels (3 for RGB, 4 for RGBA)
      WrapMode wrapModeU;     // wrap mode for U coordinate
      WrapMode wrapModeV;     // wrap mode for V coordinate
      bool needs_free;        // whether pixels were allocated by stb_image and need to be freed
};

struct BasicColorMaterial {
      mym::Vec3 color;
      mym::Vec3 specular_color;
      float shininess;
};

struct BasicTextureMaterial
{
      DArray<float> texture;
      DArray<float> uvMap;
      std::string texture_path; // path reported by Assimp (may be embedded like "*0")
      TextureData texture_data; // loaded texture data (pixels, dimensions, etc.)
      GLuint texture_id; // OpenGL texture ID once created by renderer
      float shininess;
};

using Material = std::variant<BasicColorMaterial, BasicTextureMaterial>;

#endif