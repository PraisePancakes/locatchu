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

struct text {
    std::string text;
    Rectangle container;
    Color color;
    int font_size;
};

struct button {
    Rectangle container;

    template <typename F>
    void on_click(F&& f) {
        if (CheckCollisionPointRec(GetMousePosition(), container))
            f();
    };

    template <typename F>
    void on_hover(F&& f) {
        if (CheckCollisionPointRec(GetMousePosition(), container))
            f();
    }
};

struct position {
    Vector2 location{0};
};
struct velocity {
    Vector2 direction{0};
};
};  // namespace loca