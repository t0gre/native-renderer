
#include <SDL3/SDL.h>
#include <GLES3/gl3.h>
#include "math.h"
#include <variant>

#include "events.h"
#include "mat4.h"
#include "raycast.h"
#include "tracy/Tracy.hpp"

using namespace mym;

Vec2 getPointerClickInClipSpace(const int mouse_x, const int mouse_y, const int canvas_width, const int canvas_height) {
    // Convert from window coordinates to normalized device coordinates (clip space)
    const float x = static_cast<float>(mouse_x) / static_cast<float>(canvas_width) * 2.0f - 1.0f;
    const float y = static_cast<float>(mouse_y) / static_cast<float>(canvas_height) * -2.0f + 1.0f;
    return { x, y };
}

Ray getWorldRayFromClipSpaceAndCamera(
    Vec2 clipSpacePoint, 
    Camera camera) {

    float x = clipSpacePoint.x;
    float y = clipSpacePoint.y;

    Vec3 nearPoint  = {x, y, -1.f};
    Vec3 farPoint  = {x, y,  1};

    const Mat4 viewMatrix = inverse(camera.transform);
    const Mat4 projectionMatrix = getProjectionMatrix(camera);
    const Mat4 viewProjInverse = inverse(multiplied(projectionMatrix, viewMatrix));

    const Vec3 worldNear = positionMultiplied(nearPoint, viewProjInverse);
    const Vec3 worldFar  = positionMultiplied(farPoint, viewProjInverse);

    auto rayOrigin = worldNear;

    const Vec3 rayDirection = subtractVectors(worldFar, worldNear);

    auto rayDirNorm = normalize(rayDirection);

    Ray worldRay = {
        .origin = rayOrigin,
        .direction = rayDirNorm
    };

    return worldRay;
}

void processEvents(WindowState& window, Camera& camera, InputState& input, Scene& scene)
{
    ZoneScoped;
    // Handle events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                window.should_close = true;
                break;

            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            {
                if (event.window.windowID == window.id)
                {
                    int width = event.window.data1; 
                    int height = event.window.data2;
                    glViewport(0, 0, width, height);

                    camera.aspect = (float)width / (float)height;

                    window.width = width;
                    window.height = height;
                    
                }
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                const auto* e = reinterpret_cast<SDL_MouseButtonEvent *>(&event);
                if (event.button.button == 1) {
                    input.pointer_down = true;
                    Vec2 pointer_position = {
                    .x = static_cast<float>(e->x),
                    .y = static_cast<float>(e->y)
                    };
                    
                    input.pointer_position = pointer_position;

                    // get mouse position ndc
                    const Vec2 pointer_clip = getPointerClickInClipSpace(
                        e->x, e->y, window.width, window.height
                    );
                    

                    // create ray
                    const Ray worldRay = getWorldRayFromClipSpaceAndCamera(
                        pointer_clip,
                        camera
                    );

                    // intersect scene
                    auto hits = rayIntersectsScene(worldRay, scene);

                    if (hits.size() == 0) break;
                        
                    // update floor with color of first hit
                    sortBySceneDepth(hits, camera);

                    const auto& clicked = hits[0];

                    // set the floor node to have the same color at the clicked thing
                    for (auto& node: scene.nodes) {
                        if (node->name == "floor") {
                            const auto floor = node;
                            if (std::holds_alternative<BasicColorMaterial>(clicked.meshInfo.value().material)) {
                                std::get<BasicColorMaterial>(floor->mesh.value().material).color = std::get<BasicColorMaterial>(clicked.meshInfo.value().material).color;
                            }
                            
                        }
                    }
                 }
                break;
            }
            case SDL_EVENT_MOUSE_MOTION:
            {
                const auto *e = reinterpret_cast<SDL_MouseMotionEvent *>(&event);
                if (input.pointer_down) {
                    
                    const float orbitSensitivity = camera.orbit.sensitivity;
                    const Vec3 orbitTarget = camera.orbit.target;
                    const float orbitRadius = camera.orbit.radius;

                    camera.orbit.azimuth -= e->xrel * orbitSensitivity;
                    camera.orbit.elevation -= e->yrel * orbitSensitivity;

                    const Vec3 newCameraPosition = calculateOrbitPosition(
                        camera.orbit.azimuth,
                        camera.orbit.elevation,
                        orbitTarget,
                        orbitRadius
                    );

                    camera.transform = lookAt(newCameraPosition, orbitTarget, camera.up);

                    const Vec2 pointer_position = {
                    .x = static_cast<float>(e->x),
                    .y = static_cast<float>(e->y)
                    };
                    
                    input.pointer_position = pointer_position;
                    
                }
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                if (event.button.button == 1) {
                    input.pointer_down = false;
                    constexpr Vec2 pointer_position ={ .x = 0, .y = 0 } ;
                    input.pointer_position = pointer_position;
                }
                break;
            }
        }

        
    }
}
