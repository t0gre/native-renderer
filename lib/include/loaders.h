#ifndef LOADER_H
#define LOADER_H

#include "data_structures.h"
#include "mesh.h"

char* get_shader_content(const char* fileName);

FloatData read_csv(const char* filename);

Vertices load_glb(const char* filename);

#endif //LOADER_H