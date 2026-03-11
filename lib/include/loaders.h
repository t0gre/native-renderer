#ifndef LOADER_H
#define LOADER_H

#include "scene.h"

char* get_shader_content(const char* fileName);

std::vector<float> read_csv(const char* filename);

SceneNode load_glb(const std::string&);

#endif //LOADER_H