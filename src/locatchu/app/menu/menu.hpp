#pragma once
#include <raylib.h>

#include <forge/forge.hpp>

#include "../components.hpp"
#include "../serialize.hpp"
#include "../states.hpp"

namespace loca {

class menu : public state_interface<APP_STATES> {
    using world_t = forge::registry<position, velocity, text, tag>;
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
        world.add_component<text>(play, text{.text = "Play",
                                             .container = Rectangle{.x = description_rect.x,
                                                                    .y = description_rect.y + description_rect.height + 10,
                                                                    .width = play_measure.x,
                                                                    .height = play_measure.y},
                                             .color = config.start_color,
                                             .font_size = config.start_font_size});
    };

    void initialize_quit_button() {
        assert(world.is_alive(play) && "play entity has not been initialized yet, quit initialization is out of order");
        quit = world.make();
        auto quit_measure = MeasureTextEx(GetFontDefault(), "Quit", config.quit_font_size, 5.f);
        Rectangle play_rect = world.get_component<text>(play).container;
        world.add_component<text>(quit, text{.text = "Quit",
                                             .container = Rectangle{.x = play_rect.x,
                                                                    .y = play_rect.y + play_rect.height + 10,
                                                                    .width = quit_measure.x,
                                                                    .height = quit_measure.y},
                                             .color = config.quit_color,
                                             .font_size = config.quit_font_size});
    };

    void initialize_description_text() {
        assert(world.is_alive(title) && "Title entity has not been initialized yet, description initialization is out of order");
        description = world.make();
        auto description_measure = MeasureTextEx(GetFontDefault(), config.description.c_str(), config.description_font_size, 5.f);
        Rectangle title_rect = world.get_component<text>(title).container;
        world.add_component<text>(description, text{.text = config.description,
                                                    .container = Rectangle{.x = title_rect.x,
                                                                           .y = title_rect.y + title_rect.height,
                                                                           .width = description_measure.x,
                                                                           .height = description_measure.y},
                                                    .color = config.description_color,
                                                    .font_size = config.description_font_size});
    };

    void render_play_button() {
        auto [play_text, play_container, play_color, play_font_size] = world.get_component<text>(play);
        play_container.x = play_container.x - play_container.width / 4;
        play_container.y = play_container.y - play_container.height / 2 - (GetScreenHeight() / 4);
        DrawText(play_text.c_str(), play_container.x, play_container.y, play_font_size, play_color);
    };

    void render_quit_button() {
        auto [quit_text, quit_container, quit_color, quit_font_size] = world.get_component<text>(quit);
        quit_container.x = quit_container.x - quit_container.width / 4;
        quit_container.y = quit_container.y - quit_container.height / 2 - (GetScreenHeight() / 4);
        DrawText(quit_text.c_str(), quit_container.x, quit_container.y, quit_font_size, quit_color);
    };

    void render_description_text() {
        auto [description_text, description_container, description_color, description_font_size] = world.get_component<text>(description);
        description_container.x = description_container.x - description_container.width / 4;
        description_container.y = description_container.y - description_container.height / 2 - (GetScreenHeight() / 4);
        DrawText(description_text.c_str(), description_container.x, description_container.y, description_font_size, description_color);
    };
    void initialize_title_text() {
        title = world.make();
        auto title_measure = MeasureTextEx(GetFontDefault(), config.title.c_str(), config.title_font_size, 5.f);
        world.add_component<text>(title, text{.text = config.title,
                                              .container = Rectangle{(float)GetScreenWidth() / 2,
                                                                     (float)GetScreenHeight() / 2,
                                                                     title_measure.x,
                                                                     title_measure.y},
                                              .color = config.title_color,
                                              .font_size = config.title_font_size});
    };

    void render_title_text() {
        auto [title_text, title_container, title_color, title_font_size] = world.get_component<text>(title);
        title_container.x = title_container.x - title_container.width / 2;
        title_container.y = title_container.y - title_container.height / 2 - (GetScreenHeight() / 4);
        DrawText(title_text.c_str(), title_container.x, title_container.y, title_font_size, title_color);
    };

   public:
    menu(const config::menu_config& config, auto callback) : state_interface<APP_STATES>(callback), config{config} {};
    void on_update(float) override {
        if (IsKeyPressed(KEY_ENTER)) this->request_transition(APP_STATES::GAME);
    };
    void on_render() override {
        ClearBackground(config.background_color);
        render_title_text();
        render_description_text();
        render_play_button();
        render_quit_button();
    };
    void on_init() override {
        initialize_title_text();
        initialize_description_text();
        initialize_play_button();
        initialize_quit_button();
    };
    void on_exit() override {};
    void on_input() override {};
    ~menu() = default;
};
};  // namespace loca