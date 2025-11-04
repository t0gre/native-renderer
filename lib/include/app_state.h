#ifndef APP_STATE_H
#define APP_STATE_H


#include <SDL3/SDL.h>
#include "camera.h"
#include "render_program.h"
#include "scene.h"
#include "input.h"

typedef struct WindowState  {
    SDL_Window* object;
    Uint32 id;
    bool should_close;
    size_t width;
    size_t height;

} WindowState;


typedef struct AppState  {
    WindowState window;
    Uint64 last_frame_time;
    Camera camera;
    InputState input;
    RenderProgram render_program;
    Scene scene;
    ShadowRenderProgram shadow_render_program;
    ShadowMap shadow_map;

} AppState;

#endif //APP_STATE