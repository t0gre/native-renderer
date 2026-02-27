#pragma once

#include "sdl_state.h"
#include "render_program.h"
#include "camera.h"

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

