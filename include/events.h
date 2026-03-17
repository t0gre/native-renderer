# pragma once

#include "sdl_state.h"
#include "camera.h"
#include "scene.h"
#include "vec.h"

struct Entity {
    size_t id;
    std::optional<std::string> name;
};

typedef struct InputState {
    bool pointer_down;
    linalg::Vec2 pointer_position;
    std::optional<Entity> selected_entity;
} InputState;


void processEvents(WindowState& window, Camera& camera, InputState& input, Scene& scene,  Arena& frame_arena);
