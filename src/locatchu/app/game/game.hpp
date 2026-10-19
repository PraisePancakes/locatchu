#pragma once
#include <raylib.h>

#include <forge/forge.hpp>
#include <iostream>

#include "../components.hpp"
#include "../serialize.hpp"
#include "../states.hpp"

namespace loca {

class game : public state_interface<APP_STATES> {
    using world_t = forge::registry<position, velocity, tag, boundingbox2d>;
    world_t world;
    forge::entity player;
    Camera2D camera{};
    const config::world_config config;

   public:
    game(const config::world_config& config, auto transition_callback) : state_interface<APP_STATES>(transition_callback), config{config} {};
    void on_init() override {
        player = world.make();
        auto [p, _, _, _] = world.add_component<position, velocity, tag, boundingbox2d>(player,
                                                                                        position{GetScreenWidth() / 2.f, GetScreenHeight() / 2.f},
                                                                                        velocity{0, 0},
                                                                                        tag::PLAYER,
                                                                                        boundingbox2d{.box = {.x = GetScreenWidth() / 2.f,
                                                                                                              .y = GetScreenHeight() / 2.f,
                                                                                                              .width = config.player_radius * 2,
                                                                                                              .height = config.player_radius * 2}});
        camera = Camera2D{
            .offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f},
            .target = p.location,
            .rotation = 0.0f,
            .zoom = 1.0f,
        };
        auto enemy = world.make();
        world.add_component<position, velocity, tag, boundingbox2d>(enemy,
                                                                    position{p.location.x + 50, p.location.y + 50},
                                                                    velocity{0, 0},
                                                                    tag::ENEMY,
                                                                    boundingbox2d{.box = {.x = p.location.x + 50,
                                                                                          .y = p.location.y + 50,
                                                                                          .width = config.enemy_radius * 2,
                                                                                          .height = config.enemy_radius * 2}});
    };

    void on_exit() override {};

    void on_input() override {
        auto& vel = world.get_component<velocity>(player);
        Vector2 dir{0};
        if (IsKeyDown(KEY_A)) dir.x--;
        if (IsKeyDown(KEY_W)) dir.y--;
        if (IsKeyDown(KEY_S)) dir.y++;
        if (IsKeyDown(KEY_D)) dir.x++;
        dir = Vector2Normalize(dir);
        if (IsKeyDown(KEY_LEFT_SHIFT)) dir *= config.player_running_speed_modifier;
        vel.direction = (dir * config.player_speed);
    };

    void on_update(float dt) override {
        auto& camera_position = world.get_component<const position>(player);
        camera.target = camera_position.location;

        auto v = world.view<position, velocity, tag, boundingbox2d>();
        v.each([&dt](position& p, velocity& v, tag& t, boundingbox2d& bb) {
            p.location = Vector2Add(
                p.location,
                Vector2Scale(v.direction, dt));
            bb.box.x = p.location.x - bb.box.width / 2;
            bb.box.y = p.location.y - bb.box.height / 2;
        });
        for (auto it = v.begin(); it != v.end(); ++it) {
            for (auto it2 = v.begin(); it2 != v.end(); ++it2) {
                if (it != it2) {
                    auto [pos1, vel1, _, bb1] = world.get_component<position, velocity, tag, boundingbox2d>(it.get_value());
                    auto [pos2, vel2, _, bb2] = world.get_component<position, velocity, tag, boundingbox2d>(it2.get_value());
                    if (CheckCollisionRecs(bb1.box, bb2.box)) {
                        vel1.direction = vel1.direction * -1;
                        vel2.direction = vel2.direction * -1;
                    };
                }
            }
        };
        // aabb collision
    };
    void on_render() override {
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);
        auto v = world.view<const position, const velocity, const tag, const boundingbox2d>();
        v.each([&](const position& p, const velocity& v, const tag& t, const boundingbox2d& collider) {
            DrawCircle(p.location.x,
                       p.location.y,
                       t.type == tag::PLAYER ? config.player_radius : config.enemy_radius,
                       t.type == tag::PLAYER ? config.player_color : config.enemy_color);
            DrawRectangleLines(collider.box.x, collider.box.y, collider.box.width, collider.box.height, RED);
        });
        EndMode2D();
    };
    game(const game&) = delete;
    game(game&&) = default;
    game& operator=(const game&) = delete;
    ~game() {};
};
};  // namespace loca