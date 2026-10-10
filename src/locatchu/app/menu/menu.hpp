#pragma once
#include <raylib.h>

#include <forge/forge.hpp>

#include "../components.hpp"
#include "../serialize.hpp"
#include "../states.hpp"

namespace loca {

class menu : public state_interface {
    config::menu_config config;
    forge::registry<position, velocity> world;

   public:
    menu(const config::menu_config& config) : config{config} {};
    void on_update(float) override {};
    void on_render() override {
        ClearBackground(config.background_color);
        DrawText(config.title.c_str(), (GetScreenWidth() / 2) - 50, (GetScreenHeight() / 2) - 50, config.title_font_size, config.title_color);
        DrawText(config.description.c_str(), (GetScreenWidth() / 2), (GetScreenHeight() / 2), config.description_font_size, config.description_color);
        DrawText("Start", (GetScreenWidth() / 2) - 10, (GetScreenHeight() / 2) + 50, config.start_font_size, config.start_color);
        DrawText("Quit", (GetScreenWidth() / 2) - 10, (GetScreenHeight() / 2) + 80, config.quit_font_size, config.quit_color);
    };
    void on_init() override {

    };
    void on_exit() override {};
    void on_input() override {};
    ~menu() = default;
};
};  // namespace loca