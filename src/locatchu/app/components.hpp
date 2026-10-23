#pragma once
#include <raymath.h>

#include <iostream>
namespace loca {

struct tag {
    enum {
        PLAYER,
        ENEMY,
        PARTICLE,
        BULLET
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

struct particle {
    position pos;
    float radius;
    Color color;
};
struct boundingbox2d {
    Rectangle box;
    boundingbox2d(Rectangle r) : box(r) {}
};

struct bullet_lifetime {
    float time{2.f};
};

struct health {
    int current;
    int max;
};

struct aggro {
    float radius;
};
};  // namespace loca