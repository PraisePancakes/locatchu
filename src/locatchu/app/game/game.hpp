#pragma once
#include <raylib.h>

#include <forge/forge.hpp>
#include <iostream>
#include <unordered_set>

#include "../components.hpp"
#include "../serialize.hpp"
#include "../states.hpp"

namespace loca {

class game : public state_interface<APP_STATES> {
    using world_t = forge::registry<position, velocity, tag, boundingbox2d, bullet_lifetime, health, aggro>;
    world_t world;
    forge::entity player;
    Camera2D camera{};
    const config::world_config config;

    void resolve_aabb_collisions(float dt) {
        auto v = world.view<position, boundingbox2d>();
        for (auto it = v.begin(); it != v.end(); ++it) {
            auto it2 = it;
            ++it2;
            for (; it2 != v.end(); ++it2) {
                auto [pos1, bb1] = world.get_component<position, boundingbox2d>(it.get_value());
                auto [pos2, bb2] = world.get_component<position, boundingbox2d>(it2.get_value());
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
        };
    }
    void resolve_bullet_collisions(float dt) {
        auto v = world.view<tag>();
        std::vector<forge::entity> bullets;
        v.each([&bullets](const forge::entity e, const tag& t) {
            if (t.type == tag::BULLET) bullets.push_back(e);
        });
        auto ev = world.view<boundingbox2d, health>();
        std::vector<forge::entity> to_delete;
        ev.each([&bullets, &to_delete, this](const forge::entity e, const boundingbox2d& bb, health& h) {
            for (const auto& b : bullets) {
                auto& bullet_box = world.get_component<boundingbox2d>(b);
                if (CheckCollisionRecs(bullet_box.box, bb.box)) {
                    h.current--;
                    to_delete.push_back(b);
                    break;
                }
            }
        });
        for (auto& e : to_delete) {
            world.destroy(e);
        }
    }

    void update_health() {
        std::vector<forge::entity> to_delete;
        auto v = world.view<health>();
        v.each([&to_delete](const forge::entity e, const health& h) {
            if (h.current <= 0) to_delete.push_back(e);
        });
        for (auto& e : to_delete) {
            world.destroy(e);
        };
    };
    void update_bulletlifetimes(float dt) {
        std::vector<forge::entity> to_delete;
        auto v = world.view<bullet_lifetime>();
        v.each([&to_delete, dt](const forge::entity e, bullet_lifetime& bl) {
            bl.time -= dt;
            if (bl.time <= 0) to_delete.push_back(e);
        });
        for (auto e : to_delete) {
            world.destroy(e);
        }
    }
    void update_camera_position() {
        auto& camera_position = world.get_component<const position>(player);
        camera.target = camera_position.location;
    }
    void update_entity_position(float dt) {
        auto v = world.view<position, velocity, tag, boundingbox2d>();
        v.each([&dt](position& p, velocity& v, tag& t, boundingbox2d& bb) {
            p.location = Vector2Add(p.location, Vector2Scale(v.direction, dt));
            bb.box.x = p.location.x - bb.box.width / 2;
            bb.box.y = p.location.y - bb.box.height / 2;
        });
    }

    void update_aggro() {
        auto v = world.view<position, velocity, const aggro>();
        v.each([this](position& enemy_p, velocity& enemy_v, const aggro& a) {
            auto& player_pos = this->world.get_component<position>(player);
            if (Vector2Distance(player_pos.location, enemy_p.location) < a.radius) {
                Vector2 dir{0};
                if (player_pos.location.x < enemy_p.location.x) dir.x--;
                if (player_pos.location.x > enemy_p.location.x) dir.x++;
                if (player_pos.location.y < enemy_p.location.y) dir.y--;
                if (player_pos.location.y > enemy_p.location.y) dir.y++;
                dir = Vector2Normalize(dir);
                enemy_v.direction = dir * config.enemy_speed;
            } else {
                enemy_v.direction = {0};
            }
        });
    };

    void render_entities() {
        auto v = world.view<const position, const tag, const boundingbox2d>();
        v.each([&](const position& p, const tag& t, const boundingbox2d& collider) {
            DrawCircle(p.location.x,
                       p.location.y,
                       to_radius(t),
                       to_color(t));
            DrawRectangleLines(collider.box.x, collider.box.y, collider.box.width, collider.box.height, RED);  // just for testing, maybe make a dev toggle for this
        });
    };

    void render_aggro_radius() {
        auto v = world.view<const position, const aggro>();
        v.each([](const position& p, const aggro& a) {
            DrawCircleLines(p.location.x, p.location.y, a.radius, RED);
        });
    };

    void render_health() {
        auto v = world.view<const position, const boundingbox2d, const health>();
        v.each([](const position& p, const boundingbox2d& bb, const health& h) {
            const int width = static_cast<int>(bb.box.width * (static_cast<float>(h.current) / h.max));
            const int x = p.location.x - bb.box.width / 2;
            const int y = p.location.y - bb.box.height;
            const int height = 10;

            if (width >= (bb.box.width / 2 + bb.box.width / 4)) DrawRectangle(x, y, width, height, GREEN);
            if (width < (bb.box.width / 2 + bb.box.width / 4) && width >= width / 2) DrawRectangle(x, y, width, height, ORANGE);
            if (width < bb.box.width / 2) DrawRectangle(x, y, width, height, RED);
        });
    }
    float to_radius(const tag& t) {
        switch (t.type) {
            case tag::PLAYER:
                return config.player_radius;
            case tag::BULLET:
                return config.bullet_radius;
            case tag::ENEMY:
                return config.enemy_radius;
            default:
                throw std::runtime_error("unhandled to_color");
        }
    };
    Color to_color(const tag& t) {
        switch (t.type) {
            case tag::PLAYER:
                return config.player_color;
            case tag::BULLET:
                return config.bullet_color;
            case tag::ENEMY:
                return config.enemy_color;
            default:
                throw std::runtime_error("unhandled to_color");
        }
    }
    void initialize_camera() {
        camera = Camera2D{
            .offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f},
            .target = {},
            .rotation = 0.0f,
            .zoom = 1.0f,
        };
    };
    void initialize_player() {
        player = world.make();
        world.add_component<position, velocity, tag, boundingbox2d, health>(player,
                                                                            position{GetScreenWidth() / 2.f, GetScreenHeight() / 2.f},
                                                                            velocity{0, 0},
                                                                            tag::PLAYER,
                                                                            boundingbox2d{{.x = GetScreenWidth() / 2.f - config.player_radius,
                                                                                           .y = GetScreenHeight() / 2.f - config.player_radius,
                                                                                           .width = config.player_radius * 2,
                                                                                           .height = config.player_radius * 2}},
                                                                            health{.current = 100, .max = 100});
    };

    void initialize_enemy() {
        auto enemy = world.make();
        position p{float(std::rand() % (int)GetScreenWidth()), float(std::rand() % (int)GetScreenHeight())};
        world.add_component<position, velocity, tag, boundingbox2d, health, aggro>(enemy,
                                                                                   p,
                                                                                   velocity{0, 0},
                                                                                   tag::ENEMY,
                                                                                   boundingbox2d{{.x = p.location.x - config.enemy_radius,
                                                                                                  .y = p.location.y - config.enemy_radius,
                                                                                                  .width = config.enemy_radius * 2,
                                                                                                  .height = config.enemy_radius * 2}},
                                                                                   health{.current = 100, .max = 100}, aggro{.radius = config.enemy_radius * 8});
    }

   public:
    game(const config::world_config& config, auto transition_callback) : state_interface<APP_STATES>(transition_callback), config{config} {};
    void on_init() override {
        initialize_camera();
        initialize_player();
        initialize_enemy();
    };

    void on_exit() override {};

    void on_input(float dt) override {
        auto [pos, vel, pbb] = world.get_component<position, velocity, boundingbox2d>(player);
        Vector2 dir{0};
        if (IsKeyDown(KEY_A)) dir.x--;
        if (IsKeyDown(KEY_W)) dir.y--;
        if (IsKeyDown(KEY_S)) dir.y++;
        if (IsKeyDown(KEY_D)) dir.x++;
        dir = Vector2Normalize(dir);
        if (IsKeyDown(KEY_LEFT_SHIFT)) dir *= config.player_running_speed_modifier;
        vel.direction = (dir * config.player_speed);
        static float cool_down = .1f;
        const static float max_cool_down = .1f;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) cool_down -= dt;
        if (cool_down <= 0) {
            cool_down = max_cool_down;
            auto bullet = world.make();
            Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);
            Vector2 mouse_dir = Vector2Normalize(mouse_pos - pos.location);
            position bpos{Vector2Add(pos.location, mouse_dir * pbb.box.width)};
            velocity bvel{mouse_dir * config.bullet_speed};
            boundingbox2d bbox{{.x = bpos.location.x - config.bullet_radius,
                                .y = bpos.location.y - config.bullet_radius,
                                .width = config.bullet_radius * 2,
                                .height = config.bullet_radius * 2}};
            world.add_component<bullet_lifetime, position, velocity, tag, boundingbox2d>(bullet, bullet_lifetime{.time = config.bullet_lifetime}, bpos, bvel, tag::BULLET, bbox);
        }
    };

    void on_update(float dt) override {
        update_camera_position();
        resolve_aabb_collisions(dt);
        update_entity_position(dt);
        update_bulletlifetimes(dt);
        update_health();
        update_aggro();
        resolve_bullet_collisions(dt);
    };

    void on_render() override {
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);
        render_entities();
        render_health();
        render_aggro_radius();
        EndMode2D();
    };
    game(const game&) = delete;
    game(game&&) = default;
    game& operator=(const game&) = delete;
    ~game() {};
};
};  // namespace loca