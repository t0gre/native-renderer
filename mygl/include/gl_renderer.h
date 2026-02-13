#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include "sdl_state.h"

WindowState initWindow(const char* title);

class GlRenderer {  
      
    private:             
        BasicColorRenderProgram basic_color_render_program;
        TextureRenderProgram texture_render_program;

        // Shadow map setup
        ShadowMap shadow_map;
        ShadowRenderProgram shadow_render_program;


    public:
        GlRenderer();
        void drawGl(
                WindowState window, 
                Camera camera, 
                Scene scene 
            );

};



#endif //GL_RENDERER_H