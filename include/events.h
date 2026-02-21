# pragma once

#include "sdl_state.h"
#include "input.h"
#include "camera.h"
#include "scene.h"


void processEvents(WindowState& window, Camera& camera, InputState& input, Scene& scene, AppState& appState);
