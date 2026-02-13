#ifndef EVENTS_H
#define EVENTS_H

#include "sdl_state.h"
#include "vec.h"

void processEvents(WindowState& window, Camera& camera, InputState& input, Scene& scene, AppState& appState);

#endif //EVENTS_H