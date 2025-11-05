
#include <SDL3/SDL.h>
#include <GLES3/gl3.h>
#include "math.h"

#include "events.h"
#include "mat4.h"
#include "raycast.h"

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

    const Mat4 viewMatrix = m4inverse(camera.transform);
    const Mat4 projectionMatrix = getProjectionMatrix(camera);
    const Mat4 viewProjInverse = m4inverse(m4multiply(projectionMatrix, viewMatrix));

    const Vec3 worldNear = m4PositionMultiply(nearPoint, viewProjInverse);
    const Vec3 worldFar  = m4PositionMultiply(farPoint, viewProjInverse);

    auto rayOrigin = worldNear;

    const Vec3 rayDirection = subtractVectors(worldFar, worldNear);

    auto rayDirNorm = normalize(rayDirection);

    Ray worldRay = {
        .origin = rayOrigin,
        .direction = rayDirNorm
    };

    return worldRay;
}

void processEvents(AppState* state)
{
    // Handle events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                state->window.should_close = true;
                break;

            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            {
                if (event.window.windowID == state->window.id)
                {
                    int width = event.window.data1; 
                    int height = event.window.data2;
                    glViewport(0, 0, width, height);

                    state->camera.aspect = (float)width / (float)height;

                    state->window.width = width;
                    state->window.height = height;
                    
                }
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                const auto* e = reinterpret_cast<SDL_MouseButtonEvent *>(&event);
                if (event.button.button == 1) {
                    state->input.pointer_down = true;
                    Vec2 pointer_position = {
                    .x = static_cast<float>(e->x),
                    .y = static_cast<float>(e->y)
                    };
                    
                    state->input.pointer_position = pointer_position;

                    // get mouse position ndc
                    const Vec2 pointer_clip = getPointerClickInClipSpace(
                        e->x, e->y, state->window.width, state->window.height
                    );
                    

                    // create ray
                    const Ray worldRay = getWorldRayFromClipSpaceAndCamera(
                        pointer_clip,
                        state->camera
                    );

                    // intersect scene
                    auto hits = rayIntersectsScene(worldRay, state->scene);

                    if (hits.empty()) break;
                        
                    // update floor with color of first hit
                    auto sortedHits = sortBySceneDepth(hits, state->camera);

                    const auto& clicked = sortedHits[0];

                    // set the floor node to have the same color at the clicked thing
                    for (auto& node: state->scene.nodes) {
                        if (node.name == "floor") {
                            const auto floor = &node;
                            floor->mesh.value().material.color = clicked.meshInfo.value().material.color;
                        }
                    }
                 }
                break;
            }
            case SDL_EVENT_MOUSE_MOTION:
            {
                const auto *e = reinterpret_cast<SDL_MouseMotionEvent *>(&event);
                if (state->input.pointer_down) {
                    
                    const float orbitSensitivity = state->camera.orbit.sensitivity;
                    const Vec3 orbitTarget = state->camera.orbit.target;
                    const float orbitRadius = state->camera.orbit.radius;

                    state->camera.orbit.azimuth -= e->xrel * orbitSensitivity;
                    state->camera.orbit.elevation -= e->yrel * orbitSensitivity;

                    const Vec3 newCameraPosition = calculateOrbitPosition(
                        state->camera.orbit.azimuth,
                        state->camera.orbit.elevation,
                        orbitTarget,
                        orbitRadius
                    );

                    state->camera.transform = m4lookAt(newCameraPosition, orbitTarget, state->camera.up);

                    const Vec2 pointer_position = {
                    .x = static_cast<float>(e->x),
                    .y = static_cast<float>(e->y)
                    };
                    
                    state->input.pointer_position = pointer_position;
                    
                }
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                if (event.button.button == 1) {
                    state->input.pointer_down = false;
                    constexpr Vec2 pointer_position ={ .x = 0, .y = 0 } ;
                    state->input.pointer_position = pointer_position;
                }
                break;
            }
        }

        
    }
}
