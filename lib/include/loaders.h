#ifndef LOADER_H
#define LOADER_H

#include "data_structures.h"

char* get_shader_content(const char* fileName);

FloatData read_csv(const char* filename);

#endif //LOADER_H