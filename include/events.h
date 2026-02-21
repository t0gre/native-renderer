# pragma once

#include "sdl_state.h"
#include "input.h"
#include "camera.h"
#include "scene.h"


struct Entity {
    size_t id;
    std::optional<std::string> name;
};

struct AppState {
    std::optional<Entity> selected_entity;
};


void processEvents(WindowState& window, Camera& camera, InputState& input, Scene& scene, AppState& appState);
