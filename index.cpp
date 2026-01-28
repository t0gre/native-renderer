#include "std_imports.h"
#include "mat4.h"
#include "math_utils.h"
#include "camera.h"
#include "loaders.h"
#include "app_state.h"
#include "scene.h"
#include "events.h"

WindowState initWindow(const char* title)
{
    
    SDL_Init(SDL_INIT_VIDEO < 0);

    GLsizei initial_window_height = 700;
    GLsizei initial_window_width = 1000;

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



void draw(
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


void updateScene(Scene* scene, float dt) {
    Mat4 rotator = m4yRotation(PI / (dt * 10));
    Vec4 oldMatrix = { 
        .x = scene->point_light.position.x,
        .y = scene->point_light.position.y,
        .z = scene->point_light.position.z,
        .w = 0.0
    };
    Vec4 newMatrix = m4vectorMultiply(oldMatrix, rotator);
    scene->point_light.position = (Vec3){
        .x = newMatrix.x,
        .y = newMatrix.y,
        .z = newMatrix.z
    };
    
}

void mainLoop(void* mainLoopArg) 
{   
   
    AppState* state = (AppState*)mainLoopArg;

    // calculate deltaTime
    const Uint64 now = SDL_GetPerformanceCounter();
    const Uint64 last = state->last_frame_time;

    const double deltaTime = ((now - last)*1000 / (double)SDL_GetPerformanceFrequency() );
    state->last_frame_time = now;

    // log errors
    const char* error = SDL_GetError();
    if (error[0] != '\0') {
        puts(error);
        SDL_ClearError();
    }
   
    updateScene(&state->scene, deltaTime);

    processEvents(state);
     
    draw(
        state->window, 
        state->camera, 
        &state->scene, 
        state->render_program, 
        state->shadow_render_program,
        state->shadow_map
    );

}

int main(int argc, char** argv)
{

    InputState input = {
        .pointer_down = false,
        .pointer_position = { 0 }
    };

    WindowState window = initWindow("Tom");
       
    // Initialize shader and geometry
    RenderProgram render_program = initShader();

    // Shadow map setup
    ShadowMap shadowMap = createShadowMap();
    ShadowRenderProgram shadowRenderProgram = initShadowRenderProgram();

    // create lights
    AmbientLight ambient_light = {
        .color = { .r = 0.1f, .g = 0.1f, .b = 0.1f }
    };

    DirectionalLight directional_light = {
        .color = { .r = 0.5f, .g = 0.5f, .b = 0.5f},
        .rotation = { .x = 0.0f, .y = -1.0f, .z = -1.0f},  // pointing down and forward at 45 degrees
    };

    PointLight point_light = {
        .color = { .r = 0.3f, .g = 0.3f, .b = 0.3f},
        .position = { .x = 0.f, .y = 5.0f, .z = 5.f },
        .constant = 1.0f,
        .linear = 0.009f,
        .quadratic = 0.032f
    };

    // TODO do these need to be cleaned up?
    FloatData normals = read_csv("assets/normals.txt");
    FloatData positions = read_csv("assets/positions.txt");

    Vertices vertices;

    vertices.vertex_count = positions.count / 3;

    for (size_t i = 0; i < positions.count; i ++) {
        vertices.positions.push_back(positions.data[i]);
        vertices.normals.push_back(normals.data[i]);    
    }

    SceneNode tree_shape = initSceneNode(m4fromPositionAndEuler(
            (Vec3){ .x = 0.f, .y = 0.f, .z = 0.f }, 
            (Vec3){  .x = 0.f, .y = PI / 2.f, .z = 0.f }),
            (Mesh){
                .vertices =vertices,
                .material = {
                    .color = { .r = 0.1, .g = 0.7, .b = 0.1},
                    .specular_color = { .r = 0.2, .g = 0.2, .b = 0.2},
                    .shininess = 0.5f
                },
            },
            "green tree"
        );
    
    SceneNode tree_shape1 = initSceneNode(m4fromPositionAndEuler(
            (Vec3){ .x = 5.f, .y = 0.f, .z = 0.f }, 
            (Vec3){  .x = 0.f, .y = PI / 2.f, .z = 0.f }),
            (Mesh){
                .vertices =vertices,
                .material = {
                    .color = { .r = 0.8, .g = 0.8, .b = 0.8},
                    .specular_color = { .r = 0.2, .g = 0.2, .b = 0.2},
                    .shininess = 0.9f
                },
            },
            "grey tree"
        );
    
    SceneNode tree_shape2 = initSceneNode(m4fromPositionAndEuler(
            (Vec3){ .x = 5.f, .y = 0.f, .z = 0.f }, 
            (Vec3){  .x = 0.f, .y = PI / 2.f, .z = 0.f }),
            (Mesh){
                .vertices =vertices,
                .material = {
                    .color = { .r = 0.1, .g = 0.5, .b = 0.8},
                    .specular_color = { .r = 0.2, .g = 0.2, .b = 0.2},
                    .shininess = 0.9f
                }, 
            },
            "blue tree"
        );
    
    setParent(tree_shape1, tree_shape2);
    setParent(tree_shape2, tree_shape);

    float floor_positions_data[18] = {
            -1000.f ,0.f, -1000.f, // back left
            -1000.f ,0.f, 1000.f, // front left
            1000.f ,0.f, -1000.f, // back right
            -1000.f ,0.f, 1000.f, // front left
            1000.f ,0.f, 1000.f, // front right
            1000.f ,0.f, -1000.f, // back right
        };

    float floor_normals_data[18] = {
            0.f ,1.f, 0.f,
            0.f ,1.f, 0.f,
            0.f ,1.f, 0.f,
            0.f ,1.f, 0.f,
            0.f ,1.f, 0.f,
            0.f ,1.f, 0.f,
        };

    Vertices floor_vertices;

    for (size_t i = 0; i < 18; i ++) {
        floor_vertices.positions.push_back(floor_positions_data[i]);
        floor_vertices.normals.push_back(floor_normals_data[i]);    
    }

    floor_vertices.vertex_count = 6;

    SceneNode floor_model = initSceneNode(m4fromPositionAndEuler(
            (Vec3){ .x = 0.f, .y = 0.0f, .z = 0.f }, 
            (Vec3) { .x = 0.f, .y = 0.f, .z = 0.f }),
            (Mesh){
                .vertices =floor_vertices,
                .material = {
                    .color = { .r = 0.9, .g = 0.7, .b = 0.1},
                    .specular_color = { .r = 0.9, .g = 0.9, .b = 0.9},
                    .shininess = 1000.f
                },  
            },
            "floor"
        );

    auto scene_nodes = DArray<SceneNode*>();
    scene_nodes.push_back(&tree_shape);
    scene_nodes.push_back(&floor_model);

    std::string gorilla_path = "assets/gorila.glb";
    SceneNode gorilla = load_glb(gorilla_path);
    gorilla.name = "gorilla";
    gorilla.local_transform = m4translate(gorilla.local_transform, -5.0f, 0.f, 0.f);

    updateWorldTransform(&gorilla);

    scene_nodes.push_back(&gorilla);
    
    Scene scene =  { 
        .nodes = scene_nodes,
        .ambient_light = ambient_light,
        .directional_light = directional_light,
        .point_light = point_light,
        };


    Vec3 up = { .x = 0.f, .y = 1.f, .z = 0.f };
    Orbit orbit = {
        .azimuth = - PI * 0.2f,
        .elevation = 3.f * PI / 4.f,
        .sensitivity = 0.01f,
        .radius = 15.f,
        .target = {-3.f, 2.f, -2.f}
    };

    Vec3 cameraPosition = calculateOrbitPosition(
        orbit.azimuth, 
        orbit.elevation,
        orbit.target,   
        orbit.radius
    );

    // create a camera
    const Camera camera = {
        .field_of_view_radians = 1.f,
        .aspect = (float)window.width / (float)window.height, 
        .near = 1.f,
        .far = 2000.f, 
        .up = up, 
        .transform = m4lookAt(cameraPosition, orbit.target, up),
        .orbit = orbit
        };

    Uint64 now = SDL_GetPerformanceCounter();
    
    AppState state = {
        .window = window,
        .last_frame_time = now,
        .camera = camera,
        .input = input,
        .render_program = render_program,
        .scene = scene,
        .shadow_render_program = shadowRenderProgram,
        .shadow_map = shadowMap
    };

    while(!state.window.should_close) {
        mainLoop(&state);
    }

    return 0;
}