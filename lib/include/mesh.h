#ifndef MESH_H
#define MESH_H

#include <optional>
#include <assert.h>
#include "camera.h"
#include "light.h"
#include "vec.h"
#include "material.h"
#include "mystl.hpp"

struct Vertices {
  size_t vertex_count;
  DArray<float> positions;
  DArray<float> normals;
  DArray<unsigned int> indices;
  size_t index_count;
  // add textcoords and indices
};


struct Mesh {
  Vertices vertices;
  Material material;
  std::optional<int> id; // the vao id once the mesh has been inited
}; 



#endif //MESH_H