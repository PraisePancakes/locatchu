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
                                                                                        boundingbox2d{.box = {.x = GetScreenWidth() / 2.f - config.player_radius,
                                                                                                              .y = GetScreenHeight() / 2.f - config.player_radius,
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
                                                                    boundingbox2d{.box = {.x = p.location.x + 50 - config.enemy_radius,
                                                                                          .y = p.location.y + 50 - config.enemy_radius,
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
            auto it2 = it;
            ++it2;
            for (; it2 != v.end(); ++it2) {
                if (it != it2) {
                    auto [pos1, vel1, _, bb1] = world.get_component<position, velocity, tag, boundingbox2d>(it.get_value());
                    auto [pos2, vel2, _, bb2] = world.get_component<position, velocity, tag, boundingbox2d>(it2.get_value());
                    if (CheckCollisionRecs(bb1.box, bb2.box)) {
                        float xolap = std::min(bb1.box.x + bb1.box.width, bb2.box.x + bb2.box.width) - std::max(bb1.box.x, bb2.box.x);
                        float yolap = std::min(bb1.box.y + bb1.box.height, bb2.box.y + bb2.box.height) - std::max(bb1.box.y, bb2.box.y);
                        if (xolap < yolap) {
                            const float s = pos1.location.x < pos2.location.x ? -1.f : 1.f;
                            pos1.location.x += s * xolap * 0.5;
                            pos2.location.x -= s * xolap * 0.5;
                        } else {
                            const float s = pos1.location.y < pos2.location.y ? -1.f : 1.f;
                            pos1.location.y += s * yolap * 0.5;
                            pos2.location.y -= s * yolap * 0.5;
                        }
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
            DrawRectangleLines(collider.box.x, collider.box.y, collider.box.width, collider.box.height, RED);  // just for testing, maybe make a dev toggle for this
        });
        EndMode2D();
    };
    game(const game&) = delete;
    game(game&&) = default;
    game& operator=(const game&) = delete;
    ~game() {};
};
};  // namespace loca