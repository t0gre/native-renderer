#ifndef LOADER_H
#define LOADER_H

#include "data_structures.h"
#include "mesh.h"
#include "scene.h"

char* get_shader_content(const char* fileName);

FloatData read_csv(const char* filename);

SceneNode load_glb(const std::string&);

#endif //LOADER_H