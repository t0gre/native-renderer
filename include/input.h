#pragma once

#include "vec.h"

typedef struct InputState {
    bool pointer_down;
    mym::Vec2 pointer_position;
} InputState;