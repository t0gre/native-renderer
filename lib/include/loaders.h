#ifndef LOADER_H
#define LOADER_H

#include "mesh.h"
#include "scene.h"
#include "mystl.hpp"

char* get_shader_content(const char* fileName);

DArray<float> read_csv(const char* filename);

SceneNode load_glb(const std::string&);

#endif //LOADER_H