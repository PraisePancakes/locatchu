#pragma once
#include <raylib.h>

#include <forge/forge.hpp>
#include <unordered_set>

#include "../components.hpp"
#include "../serialize.hpp"
#include "../states.hpp"

namespace loca {

class menu : public state_interface<APP_STATES> {
    constexpr static std::size_t TEXT_Y_PADDING = 10;

    struct {
        const std::size_t PARTICLE_COUNT = 100;
        const int X_SPACE = GetScreenWidth() / (PARTICLE_COUNT - 1);
        const float AMP = GetScreenHeight() / 2;
        const int PHASE_SHIFT = 0;
        const float FREQ = .01;
        const std::size_t MIN_PARTICLE_RADIUS = 4.f;
        const std::size_t MAX_PARTICLE_RADIUS = 6.f;
        const float PARTICLE_SPEED = 40.f;
    } particle_config;

    using world_t = forge::registry<particle, text, button, tag>;
    config::menu_config config;
    world_t world;
    forge::entity title;
    forge::entity description;
    forge::entity play;
    forge::entity quit;

    void initialize_play_button() {
        assert(world.is_alive(description) && "description entity has not been initialized yet, play initialization is out of order");
        play = world.make();
        auto play_measure = MeasureTextEx(GetFontDefault(), "Play", config.start_font_size, 5.f);
        Rectangle description_rect = world.get_component<text>(description).container;
        Rectangle play_rect{.x = description_rect.x, .y = description_rect.y + description_rect.height + TEXT_Y_PADDING, .width = play_measure.x, .height = play_measure.y};
        world.add_component<text>(play, text{.text = "Play", .container = play_rect, .color = config.start_color, .font_size = config.start_font_size});
        world.add_component<button>(play, button{.container = play_rect});
    };

    void initialize_quit_button() {
        assert(world.is_alive(play) && "play entity has not been initialized yet, quit initialization is out of order");
        quit = world.make();
        auto quit_measure = MeasureTextEx(GetFontDefault(), "Quit", config.quit_font_size, 5.f);
        Rectangle play_rect = world.get_component<text>(play).container;
        Rectangle quit_rect{.x = play_rect.x, .y = play_rect.y + play_rect.height + TEXT_Y_PADDING, .width = quit_measure.x, .height = quit_measure.y};
        world.add_component<text>(quit, text{.text = "Quit", .container = quit_rect, .color = config.quit_color, .font_size = config.quit_font_size});
        world.add_component<button>(quit, button{.container = quit_rect});
    };

    void initialize_description_text() {
        assert(world.is_alive(title) && "Title entity has not been initialized yet, description initialization is out of order");
        description = world.make();
        auto description_measure = MeasureTextEx(GetFontDefault(), config.description.c_str(), config.description_font_size, 5.f);
        Rectangle title_rect = world.get_component<text>(title).container;
        world.add_component<text>(description, text{.text = config.description,
                                                    .container = Rectangle{.x = title_rect.x,
                                                                           .y = title_rect.y + title_rect.height + TEXT_Y_PADDING,
                                                                           .width = description_measure.x,
                                                                           .height = description_measure.y},
                                                    .color = config.description_color,
                                                    .font_size = config.description_font_size});
    };

    void render_play_button() {
        auto [play_text, play_container, play_color, play_font_size] = world.get_component<text>(play);
        DrawText(play_text.c_str(), play_container.x, play_container.y, play_font_size, play_color);
    };

    void render_quit_button() {
        auto [quit_text, quit_container, quit_color, quit_font_size] = world.get_component<text>(quit);
        DrawText(quit_text.c_str(), quit_container.x, quit_container.y, quit_font_size, quit_color);
    };

    void render_description_text() {
        auto [description_text, description_container, description_color, description_font_size] = world.get_component<text>(description);
        DrawText(description_text.c_str(), description_container.x, description_container.y, description_font_size, description_color);
    };
    void initialize_title_text() {
        title = world.make();
        auto title_measure = MeasureTextEx(GetFontDefault(), config.title.c_str(), config.title_font_size, 5.f);
        world.add_component<text>(title, text{.text = config.title,
                                              .container = Rectangle{(float)GetScreenWidth() / 2 - title_measure.x / 2,
                                                                     (float)GetScreenHeight() / 2 - title_measure.y / 2 - (GetScreenHeight() / 4),
                                                                     title_measure.x,
                                                                     title_measure.y},
                                              .color = config.title_color,
                                              .font_size = config.title_font_size});
    };

    void initialize_particles() {
        for (int i = 0; i < particle_config.PARTICLE_COUNT; i++) {
            float x = i * particle_config.X_SPACE;
            float y = (particle_config.AMP * sin(particle_config.FREQ * x + particle_config.PHASE_SHIFT) + GetScreenHeight() / 2);
            auto p = world.make();
            position pos{.location = Vector2{.x = x, .y = y}};
            float radius = particle_config.MIN_PARTICLE_RADIUS + (std::rand() % (particle_config.MAX_PARTICLE_RADIUS - particle_config.MIN_PARTICLE_RADIUS));
            world.add_component<particle, tag>(p, particle{.pos = pos, .radius = radius, .color = RAYWHITE}, tag{.type = tag::PARTICLE});
        };
    };

    void render_particles() {
        auto vw = world.view<const particle>();
        vw.each([](const particle& p) {
            DrawCircle(p.pos.location.x, p.pos.location.y, p.radius, p.color);
        });
    }

    void render_title_text() {
        auto [title_text, title_container, title_color, title_font_size] = world.get_component<text>(title);
        DrawText(title_text.c_str(), title_container.x, title_container.y, title_font_size, title_color);
    };

   public:
    menu(const config::menu_config& config, auto transition_callback) : state_interface<APP_STATES>(transition_callback), config{config} {};
    void on_update(float dt) override {
        auto vw = world.view<particle>();
        vw.each([this, dt](particle& p) {
            p.pos.location.x += dt * particle_config.PARTICLE_SPEED;
            if (p.pos.location.x > GetScreenWidth()) p.pos.location.x -= GetScreenWidth();
            p.pos.location.y = particle_config.AMP * std::sin(particle_config.FREQ * p.pos.location.x + particle_config.PHASE_SHIFT) + GetScreenHeight() / 2;
        });
    };
    void on_render() override {
        ClearBackground(config.background_color);
        render_particles();
        render_title_text();
        render_description_text();
        render_play_button();
        render_quit_button();
    };
    void on_init() override {
        initialize_particles();
        initialize_title_text();
        initialize_description_text();
        initialize_play_button();
        initialize_quit_button();
    };
    void on_exit() override {};
    void on_input() override {
        if (IsKeyPressed(KEY_ENTER)) this->request_transition(APP_STATES::GAME);
        auto [play_text, play_button] = world.get_component<text, button>(play);
        play_button.on_hover([&play_text]() {
                       play_text.color = BLACK;
                   })
            .on_release([&play_text, this]() {
                play_text.color = this->config.start_color;
            })
            .on_click([this]() {
                this->request_transition(APP_STATES::GAME);
            });
        auto [quit_text, quit_button] = world.get_component<text, button>(quit);
        quit_button.on_hover([&quit_text]() {
                       quit_text.color = BLACK;
                   })
            .on_release([&quit_text, this]() {
                quit_text.color = this->config.quit_color;
            })
            .on_click([this]() {
                this->request_transition(APP_STATES::QUIT);
            });
    };
    ~menu() = default;
};
};  // namespace loca