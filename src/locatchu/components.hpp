#pragma once
#include <raymath.h>

#include <iostream>
namespace loca {

struct Tag {
    enum class Type {
        PLAYER,
        ENEMY
    } type;
};

struct Position {
    Vector2 location{0};
};
struct Velocity {
    Vector2 direction{0};
};
};  // namespace loca