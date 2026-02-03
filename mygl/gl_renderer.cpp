#include "gl_renderer.h"

using namespace mym;

WindowState initWindow(const char* title)
{
    
    SDL_Init(SDL_INIT_VIDEO);

    GLsizei initial_window_height = 1400;
    GLsizei initial_window_width = 2000;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* window_object = SDL_CreateWindow(title,
                         initial_window_width, 
                         initial_window_height, 
                         SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    
    const Uint32 window_id = SDL_GetWindowID(window_object);

    SDL_GL_CreateContext(window_object);
    
    // Enable VSync (try 1, fallback to -1)
    if (SDL_GL_SetSwapInterval(1) != 0) {
        printf("VSync (1) not supported, trying adaptive VSync (-1)\n");
        SDL_GL_SetSwapInterval(-1);
    }

    glClearColor(0.01f, 0.05f, 0.05f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Initialize viewport
    glViewport(0,0, initial_window_width, initial_window_height);

    const WindowState window = {
        .object = window_object, 
        .id = window_id,
        .width = static_cast<size_t>(initial_window_width),
        .height = static_cast<size_t>(initial_window_height)
        };
        
    return window;
}

GlRenderer::GlRenderer() {
        // Initialize shader and geometry
        basic_color_render_program = initShader();
        texture_render_program = initTextureShader();

        // Shadow map setup
        shadow_map = createShadowMap();
        shadow_render_program = initShadowRenderProgram();
    }


void GlRenderer::drawGl(
    WindowState window, 
    Camera camera, 
    Scene* scene 
)
{
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw shadows
    // 1. Render to shadow map
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map.framebuffer);
    glViewport(0, 0, shadow_map.size, shadow_map.size);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Disable color writes because framebuffer is depth-only (glDrawBuffers isn't available)
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glUseProgram(shadow_render_program.program);


    // Compute light's view-projection matrix (for directional light)
    Vec3 lightDirection = scene->directional_light.direction;
    float shadowDistance = 10.f;
    Vec3 lightTarget = {0.f, 0.f, 0.f};
    Vec3 lightCameraPosition = {
        lightTarget.x - lightDirection.x * shadowDistance,
        lightTarget.y - lightDirection.y * shadowDistance,
        lightTarget.z - lightDirection.z * shadowDistance
    };
    Vec3 up = {0.f, 1.f, 0.f};

    Mat4 lightView = inverse(lookAt(lightCameraPosition, lightTarget, up));
    Mat4 lightProj = orthographic(-20, 20, -20, 20, 1, 100);
    Mat4 lightViewProj = multiply(lightProj, lightView);


    for (size_t i = 0; i < scene->nodes.size(); i++) {
        drawSceneNodeShadow(scene->nodes[i], shadow_render_program, lightViewProj);
    }

    
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glUseProgram(basic_color_render_program.shader_program);


    // draw scene with shadows as input

     // 2. Render main scene
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window.width, window.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind shadow map texture to texture unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadow_map.depthTexture);
    glUniform1i(basic_color_render_program.shadow_uniform.shadow_map_location, 1);

    glUniformMatrix4fv(basic_color_render_program.shadow_uniform.light_view_location, 1,0, &lightViewProj.data[0][0]);

    // update camera uniforms
    const Mat4 projection = getProjectionMatrix(camera);
    const Mat4 view = getViewMatrix(camera);
    const Vec3 camera_position = getPositionVector(camera.transform);
    glUniformMatrix4fv(basic_color_render_program.view_uniform_location,1,0, &view.data[0][0]);  
    glUniform3fv(basic_color_render_program.view_position_uniform_location,1, &camera_position.data[0]); 
    glUniformMatrix4fv(basic_color_render_program.projection_uniform_location,1,0, &projection.data[0][0]);

    // update light uniforms
    // set ambient light
    glUniform3fv(basic_color_render_program.ambient_light_uniform.color_location,1,scene->ambient_light.color.data);

    // set directional light
    glUniform3fv(basic_color_render_program.directional_light_uniform.color_location,1,scene->directional_light.color.data);
    glUniform3fv(basic_color_render_program.directional_light_uniform.direction_location,1,scene->directional_light.direction.data);

    // set point light
    glUniform3fv(basic_color_render_program.point_light_uniform.color_location,1,scene->point_light.color.data);
    glUniform3fv(basic_color_render_program.point_light_uniform.position_location,1,scene->point_light.position.data);
    glUniform1f(basic_color_render_program.point_light_uniform.constant_location,scene->point_light.constant);
    glUniform1f(basic_color_render_program.point_light_uniform.linear_location,scene->point_light.linear);
    glUniform1f(basic_color_render_program.point_light_uniform.quadratic_location,scene->point_light.quadratic); 

    // Draw color material meshes
    for (size_t i = 0; i < scene->nodes.size(); i++) {
        drawSceneNodeBasicColor(scene->nodes[i], basic_color_render_program);
    }

    // Set up texture render program with same uniforms
    glUseProgram(texture_render_program.shader_program);
    glActiveTexture(GL_TEXTURE0);
    glUniformMatrix4fv(texture_render_program.view_uniform_location,1,0, &view.data[0][0]);  
    glUniform3fv(texture_render_program.view_position_uniform_location,1, &camera_position.data[0]); 
    glUniformMatrix4fv(texture_render_program.projection_uniform_location,1,0, &projection.data[0][0]);

    glUniformMatrix4fv(texture_render_program.shadow_uniform.light_view_location, 1,0, &lightViewProj.data[0][0]);
    glUniform1i(texture_render_program.shadow_uniform.shadow_map_location, 1);

    glUniform3fv(texture_render_program.ambient_light_uniform.color_location,1,scene->ambient_light.color.data);
    glUniform3fv(texture_render_program.directional_light_uniform.color_location,1,scene->directional_light.color.data);
    glUniform3fv(texture_render_program.directional_light_uniform.direction_location,1,scene->directional_light.direction.data);
    glUniform3fv(texture_render_program.point_light_uniform.color_location,1,scene->point_light.color.data);
    glUniform3fv(texture_render_program.point_light_uniform.position_location,1,scene->point_light.position.data);
    glUniform1f(texture_render_program.point_light_uniform.constant_location,scene->point_light.constant);
    glUniform1f(texture_render_program.point_light_uniform.linear_location,scene->point_light.linear);
    glUniform1f(texture_render_program.point_light_uniform.quadratic_location,scene->point_light.quadratic); 

    // Draw texture material meshes
    for (size_t i = 0; i < scene->nodes.size(); i++) {
        drawSceneNodeTexture(scene->nodes[i], texture_render_program);
    }

    SDL_GL_SwapWindow(window.object);
}



