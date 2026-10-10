#pragma once
#include <raymath.h>

#include <iostream>
namespace loca {

struct tag {
    enum {
        PLAYER,
        ENEMY,
        PARTICLE
    } type;
};

struct position {
    Vector2 location{0};
};
struct velocity {
    Vector2 direction{0};
};
};  // namespace loca