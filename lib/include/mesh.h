#ifndef MESH_H
#define MESH_H

#include <optional>
#include <assert.h>
#include "material.h"
#include <vector>

struct Vertices {
  size_t vertex_count;
  std::vector<float> positions;
  std::vector<float> normals;
  std::vector<unsigned int> indices;
  size_t index_count;
};


struct Mesh {
  Vertices vertices;
  Material material;
  std::optional<int> id; // the vao id once the mesh has been inited
}; 



#endif //MESH_H