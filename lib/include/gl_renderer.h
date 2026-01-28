#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include "app_state.h"

WindowState initWindow(const char* title);

void drawGl(
    WindowState window, 
    Camera camera, 
    Scene* scene, 
    RenderProgram render_program,
    ShadowRenderProgram shadow_render_program,
    ShadowMap shadow_map
);

#endif //GL_RENDERER_H