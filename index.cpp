
#include "mat4.h"
#include "math_utils.h"
#include "camera.h"
#include "loaders.h"
#include "gl_renderer.h"
#include "scene.h"
#include "events.h"


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
     
    drawGl(
        state->window, 
        state->camera, 
        &state->scene, 
        state->basic_color_render_program,
        state->texture_render_program,
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
    BasicColorRenderProgram basic_color_render_program = initShader();
    TextureRenderProgram texture_render_program = initTextureShader();

    // Shadow map setup
    ShadowMap shadowMap = createShadowMap();
    ShadowRenderProgram shadowRenderProgram = initShadowRenderProgram();

    // create lights
    AmbientLight ambient_light = {
        .color = { .r = 0.1f, .g = 0.1f, .b = 0.1f }
    };

    DirectionalLight directional_light = {
        .color = { .r = 0.5f, .g = 0.5f, .b = 0.5f},
        .direction = { .x = 0.0f, .y = -1.0f, .z = -1.0f},  // pointing down and forward at 45 degrees
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

    BasicColorMaterial greenMaterial = {
                    .color = { .r = 0.1, .g = 0.7, .b = 0.1},
                    .specular_color = { .r = 0.2, .g = 0.2, .b = 0.2},
                    .shininess = 0.5f
                };

    SceneNode green_tree = createSceneNode(m4fromPositionAndEuler(
            (Vec3){ .x = 0.f, .y = 0.f, .z = 0.f }, 
            (Vec3){  .x = 0.f, .y = PI / 2.f, .z = 0.f }),
            (Mesh){
                .vertices =vertices,
                .material = greenMaterial,
            },
            "green tree"
        );

    BasicColorMaterial greyMaterial = {
                    .color = { .r = 0.8, .g = 0.8, .b = 0.8},
                    .specular_color = { .r = 0.2, .g = 0.2, .b = 0.2},
                    .shininess = 0.9f
                };
    
    SceneNode grey_tree = createSceneNode(m4fromPositionAndEuler(
            (Vec3){ .x = 5.f, .y = 0.f, .z = 0.f }, 
            (Vec3){  .x = 0.f, .y = PI / 2.f, .z = 0.f }),
            (Mesh){
                .vertices =vertices,
                .material = greyMaterial,
            },
            "grey tree"
        );
    
    BasicColorMaterial blueMaterial = {
                    .color = { .r = 0.1, .g = 0.5, .b = 0.8},
                    .specular_color = { .r = 0.2, .g = 0.2, .b = 0.2},
                    .shininess = 0.9f
                };

    SceneNode blue_tree = createSceneNode(m4fromPositionAndEuler(
            (Vec3){ .x = 5.f, .y = 0.f, .z = 0.f }, 
            (Vec3){  .x = 0.f, .y = PI / 2.f, .z = 0.f }),
            (Mesh){
                .vertices =vertices,
                .material = blueMaterial,
            },
            "blue tree"
        );
    
    setParent(grey_tree, blue_tree);
    setParent(blue_tree, green_tree);

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

    BasicColorMaterial sandMaterial = {
                    .color = { .r = 0.9, .g = 0.7, .b = 0.1},
                    .specular_color = { .r = 0.9, .g = 0.9, .b = 0.9},
                    .shininess = 1000.f
                };  

    SceneNode floor_model = createSceneNode(m4fromPositionAndEuler(
            (Vec3){ .x = 0.f, .y = 0.0f, .z = 0.f }, 
            (Vec3) { .x = 0.f, .y = 0.f, .z = 0.f }),
            (Mesh){
                .vertices =floor_vertices,
                .material = sandMaterial
            },
            "floor"
        );

    auto scene_nodes = DArray<SceneNode*>();
    scene_nodes.push_back(&green_tree);
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
        .basic_color_render_program = basic_color_render_program,
        .texture_render_program = texture_render_program,
        .scene = scene,
        .shadow_render_program = shadowRenderProgram,
        .shadow_map = shadowMap
    };

    while(!state.window.should_close) {
        mainLoop(&state);
    }

    return 0;
}