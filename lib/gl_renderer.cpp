#include "gl_renderer.h"

WindowState initWindow(const char* title)
{
    
    SDL_Init(SDL_INIT_VIDEO < 0);

    GLsizei initial_window_height = 1400;
    GLsizei initial_window_width = 2000;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
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



void drawGl(
    WindowState window, 
    Camera camera, 
    Scene* scene, 
    RenderProgram render_program,
    ShadowRenderProgram shadow_render_program,
    ShadowMap shadow_map
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
    Vec3 lightDirection = scene->directional_light.rotation;
    float shadowDistance = 10.f;
    Vec3 lightTarget = {0.f, 0.f, 0.f};
    Vec3 lightCameraPosition = {
        lightTarget.x - lightDirection.x * shadowDistance,
        lightTarget.y - lightDirection.y * shadowDistance,
        lightTarget.z - lightDirection.z * shadowDistance
    };
    Vec3 up = {0.f, 1.f, 0.f};

    Mat4 lightView = m4inverse(m4lookAt(lightCameraPosition, lightTarget, up));
    Mat4 lightProj = m4orthographic(-20, 20, -20, 20, 1, 100);
    Mat4 lightViewProj = m4multiply(lightProj, lightView);


    for (size_t i = 0; i < scene->nodes.size(); i++) {
        drawSceneNodeShadow(scene->nodes[i], render_program, shadow_render_program, lightViewProj);
    }

    
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glUseProgram(render_program.shader_program);


    // draw scene with shadows as input

     // 2. Render main scene
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window.width, window.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind shadow map texture to texture unit 0
    // bind the shadowmap
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadow_map.depthTexture);

    glUniformMatrix4fv(render_program.shadow_uniform.light_view_location, 1,0, &lightViewProj.data[0][0]);

    // update camera uniforms
    const Mat4 projection = getProjectionMatrix(camera);
    const Mat4 view = getViewMatrix(camera);
    const Vec3 camera_position = getPositionVector(camera.transform);
    glUniformMatrix4fv(render_program.view_uniform_location,1,0, &view.data[0][0]);  
    glUniform3fv(render_program.view_position_uniform_location,1, &camera_position.data[0]); 
    glUniformMatrix4fv(render_program.projection_uniform_location,1,0, &projection.data[0][0]);

    // update light uniforms
    // set ambient light
    glUniform3fv(render_program.ambient_light_uniform.color_location,1,scene->ambient_light.color.data);

    // set directional light
    glUniform3fv(render_program.directional_light_uniform.color_location,1,scene->directional_light.color.data);
    glUniform3fv(render_program.directional_light_uniform.rotation_location,1,scene->directional_light.rotation.data);

    // set point light
    glUniform3fv(render_program.point_light_uniform.color_location,1,scene->point_light.color.data);
    glUniform3fv(render_program.point_light_uniform.position_location,1,scene->point_light.position.data);
    glUniform1f(render_program.point_light_uniform.constant_location,scene->point_light.constant);
    glUniform1f(render_program.point_light_uniform.linear_location,scene->point_light.linear);
    glUniform1f(render_program.point_light_uniform.quadratic_location,scene->point_light.quadratic); 

    
    for (size_t i = 0; i < scene->nodes.size(); i++) {
        drawSceneNode(scene->nodes[i], render_program);
    }

    SDL_GL_SwapWindow(window.object);
}