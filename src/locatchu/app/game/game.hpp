#pragma once
#include <raylib.h>

#include <forge/forge.hpp>
#include <iostream>

#include "../serialize.hpp"
#include "components.hpp"

namespace loca {
using world_t = forge::registry<position, velocity, tag>;
class game {
    world_t world;
    forge::entity player;
    Camera2D camera{};
    const config::world_config config;

    [[nodiscard]] bool init() {
        player = world.make();
        auto [p, _, _] = world.add_component<position, velocity, tag>(player, position{GetScreenWidth() / 2.f, GetScreenHeight() / 2.f}, velocity{0, 0}, tag::PLAYER);
        camera = Camera2D{
            .offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f},
            .target = p.location,
            .rotation = 0.0f,
            .zoom = 1.0f,
        };
        auto enemy = world.make();
        world.add_component<position, velocity, tag>(enemy, position{p.location.x + 50, p.location.y + 50}, velocity{0, 0}, tag::ENEMY);
        return true;
    };

    void input() {
        const static float SPEED = config.player_speed;
        auto& vel = world.get_component<velocity>(player);
        Vector2 dir{0};
        if (IsKeyDown(KEY_A)) dir.x--;
        if (IsKeyDown(KEY_W)) dir.y--;
        if (IsKeyDown(KEY_S)) dir.y++;
        if (IsKeyDown(KEY_D)) dir.x++;
        dir = Vector2Normalize(dir);
        vel.direction = (dir * SPEED);
    };

    void update(float dt) {
        auto& camera_position = world.get_component<const position>(player);
        camera.target = camera_position.location;

        auto v = world.view<position, velocity, tag>();
        v.each([&dt](position& p, velocity& v, tag& t) {
            p.location = Vector2Add(
                p.location,
                Vector2Scale(v.direction, dt));
        });
    };
    void render() {
        auto v = world.view<const position, const velocity, const tag>();
        v.each([&](const position& p, const velocity& v, const tag& t) {
            DrawCircle(p.location.x,
                       p.location.y,
                       t.type == tag::PLAYER ? config.player_radius : config.enemy_radius,
                       t.type == tag::PLAYER ? config.player_color : config.enemy_color);
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
    game(const config::world_config& config) : config{config} {};
    void run() {
        if (!init()) throw std::runtime_error("game initialization failed");
        loop();
    };
    game(const game&) = delete;
    game(game&&) = default;
    game& operator=(const game&) = delete;
    ~game() {};
};
};  // namespace loca