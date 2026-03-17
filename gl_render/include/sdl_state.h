#pragma once

#include <SDL3/SDL.h>

struct WindowState  {
    SDL_Window* object;
    SDL_GLContext context;
    Uint32 id;
    bool should_close;
    size_t width;
    size_t height;
};


