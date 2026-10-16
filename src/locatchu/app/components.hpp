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
    button& on_click(F&& f) {
        if (CheckCollisionPointRec(GetMousePosition(), container) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            f();
        return *this;
    };

    template <typename F>
    button& on_hover(F&& f) {
        if (CheckCollisionPointRec(GetMousePosition(), container))
            f();
        return *this;
    }

    template <typename F>
    button& on_release(F&& f) {
        if (!CheckCollisionPointRec(GetMousePosition(), container))
            f();
        return *this;
    }
};

struct position {
    Vector2 location{0};
};
struct velocity {
    Vector2 direction{0};
};
};  // namespace loca