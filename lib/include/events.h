#ifndef EVENTS_H
#define EVENTS_H

#include "app_state.h"
#include "vec.h"

Vec2 normalizeMousePosition(Vec2 mouse_position, Vec2 canvas_dims);

void processEvents(AppState* state);

#endif //EVENTS_H