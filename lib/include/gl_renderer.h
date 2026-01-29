#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include "app_state.h"

GlWindowState initWindow(const char* title);

void drawGl(
    GlWindowState window, 
    Camera camera, 
    Scene* scene, 
    BasicColorRenderProgram render_program,
    TextureRenderProgram texture_render_program,
    ShadowRenderProgram shadow_render_program,
    ShadowMap shadow_map
);

#endif //GL_RENDERER_H