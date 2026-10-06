#pragma once
#include <raylib.h>

#include <forge/forge.hpp>
#include <iostream>

#include "components.hpp"

namespace loca {
using world_t = forge::registry<Position, Velocity, Tag>;
class Game {
    world_t world;
    forge::entity player;
    Camera2D camera{};

    [[nodiscard]] bool init() {
        constexpr static std::size_t screen_width = 800;
        constexpr static std::size_t screen_height = 800;
        constexpr static const char* title = "locatchu";

        InitWindow(screen_width, screen_height, title);
        player = world.make();

        auto [p, _, _] = world.add_component<Position, Velocity, Tag>(player, Position{screen_width / 2, screen_height / 2}, Velocity{0, 0}, Tag::Type::PLAYER);
        camera = Camera2D{
            .offset = {screen_width / 2.0f, screen_height / 2.0f},
            .target = p.location,
            .rotation = 0.0f,
            .zoom = 1.0f,
        };
        auto enemy = world.make();
        world.add_component<Position, Velocity, Tag>(enemy, Position{p.location.x + 50, p.location.y + 50}, Velocity{0, 0}, Tag::Type::ENEMY);

        return true;
    };

    void input() {
        constexpr static float SPEED = 200;
        auto& vel = world.get_component<Velocity>(player);
        Vector2 dir{0};
        if (IsKeyDown(KEY_A)) dir.x--;
        if (IsKeyDown(KEY_W)) dir.y--;
        if (IsKeyDown(KEY_S)) dir.y++;
        if (IsKeyDown(KEY_D)) dir.x++;
        dir = Vector2Normalize(dir);
        vel.direction = (dir * SPEED);
    };

    void update(float dt) {
        auto& camera_position = world.get_component<const Position>(player);
        camera.target = camera_position.location;

        auto v = world.view<Position, Velocity, Tag>();
        v.each([&dt](Position& p, Velocity& v, Tag& t) {
            p.location = Vector2Add(
                p.location,
                Vector2Scale(v.direction, dt));
        });
    };
    void render() {
        auto v = world.view<const Position, const Velocity, const Tag>();
        v.each([](const Position& p, const Velocity& v, const Tag& t) {
            DrawCircle(p.location.x, p.location.y, 25, t.type == Tag::Type::PLAYER ? BLACK : RED);
        });
    };
    void loop() {
        while (!WindowShouldClose()) {
            float dt = GetFrameTime();
            input();
            update(dt);
            BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);
            render();
            EndMode2D();
            EndDrawing();
        };
    };

   public:
    Game() {};
    void run() {
        if (!init()) throw std::runtime_error("Error initializing game");
        loop();
    };
    Game(const Game&) = delete;
    Game(Game&&) = default;
    Game& operator=(const Game&) = delete;
    ~Game() {};
};
};  // namespace loca