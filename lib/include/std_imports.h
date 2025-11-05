#ifndef THIS_PROJECTS_STD_IMPORTS_H
#define THIS_PROJECTS_STD_IMPORTS_H

#ifdef __EMSCRIPTEN__
#include <SDL.h>
#include <GLES3/gl3.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#else 
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <stdio.h>
#endif



#endif //THIS_PROJECTS_STD_IMPORTS_H