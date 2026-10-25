#pragma once

#include <raylib.h>

#include <fstream>
#include <nlohmann/json.hpp>

#include "game/game.hpp"
#include "menu/menu.hpp"
#include "serialize.hpp"
#include "states.hpp"

namespace loca {
class application {
    config::world_config world_config{};
    config::app_config app_config{};
    config::menu_config menu_config{};
    loca::state_manager<state_interface<APP_STATES>> state_manager;
    template <typename T>
        requires(config::configuration_compatible<T>)
    [[nodiscard]] bool initialize_configuration(T& config) {
        const std::filesystem::path path = std::filesystem::path{PROJECT_ROOT} / config.configuration_path;
        std::filesystem::create_directories(path.parent_path());
        if (!std::filesystem::exists(path)) {
            std::ofstream file{path};
            if (!file.is_open()) return false;
            nlohmann::ordered_json j = config;
            file << j;
            return true;
        }
        std::ifstream file{path};
        if (!file.is_open()) return false;
        nlohmann::json data = nlohmann::ordered_json::parse(file);
        config = data.get<T>();
        return true;
    }

    template <typename... Ts>
    bool initialize_configuration_set(Ts&&... ts) {
        return (initialize_configuration(std::forward<Ts>(ts)) && ...);
    };

    [[nodiscard]] bool init() {
        SetTraceLogLevel(LOG_NONE);  // for now
        if (!initialize_configuration_set(this->menu_config, this->app_config, this->world_config)) return false;
        InitWindow(this->app_config.screen_width, this->app_config.screen_height, this->app_config.title.c_str());
        SetWindowState(FLAG_FULLSCREEN_MODE);
        auto transition_callback = [this](APP_STATES s) {
            state_manager.request_transition(s);
        };
        this->state_manager.register_state<game>(APP_STATES::GAME, world_config, transition_callback);
        this->state_manager.register_state<menu>(APP_STATES::MENU, menu_config, transition_callback);
        this->state_manager.set_current_state(APP_STATES::MENU);
        return true;
    };

   public:
    void run() {
        if (!init()) throw std::runtime_error("application initialization failed");
        while (!WindowShouldClose() && this->state_manager.get_current_id() != APP_STATES::QUIT) {
            BeginDrawing();
            float dt = GetFrameTime();

            this->state_manager.get_current_state()->on_input(dt);
            this->state_manager.get_current_state()->on_update(dt);
            this->state_manager.get_current_state()->on_render();
            EndDrawing();
        }

        CloseWindow();
    };

    ~application() {
        auto* current_state = this->state_manager.get_current_state();
        if (current_state)
            current_state->on_exit();
    };
};
};  // namespace loca