#ifndef APP_STATE_H
#define APP_STATE_H


#include <SDL3/SDL.h>
#include "camera.h"
#include "render_program.h"
#include "scene.h"
#include "input.h"

struct WindowState  {
    SDL_Window* object;
    Uint32 id;
    bool should_close;
    size_t width;
    size_t height;
};


struct AppState  {
    WindowState window;
    Uint64 last_frame_time;
    Camera camera;
    InputState input;
    Scene scene;
};

#endif //APP_STATE_H