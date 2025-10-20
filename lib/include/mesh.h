#ifndef MESH_H
#define MESH_H

#include <optional>
#include <assert.h>
#include "camera.h"
#include "light.h"
#include "vec.h"
#include "data_structures.h"
#include "material.h"

typedef struct Vertices {
  size_t vertex_count;
  float * positions;
  float * normals;
  // add textcoords and indices
} Vertices;


typedef struct Mesh {
  Vertices vertices;
  Material material;
  std::optional<int> id; // the vao id once the mesh has been inited
} Mesh;



#endif //MESH_H