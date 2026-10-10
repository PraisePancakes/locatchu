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
    enum class APP_STATES : std::uint8_t {
        GAME,
        MENU
    };
    loca::state_manager<APP_STATES, state_interface> state_manager;

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
        if (!initialize_configuration_set(this->menu_config, this->app_config, this->world_config)) return false;
        InitWindow(this->app_config.screen_width, this->app_config.screen_height, this->app_config.title.c_str());
        this->state_manager.register_state<game>(APP_STATES::GAME, world_config);
        this->state_manager.set_current_state(APP_STATES::GAME);

        return true;
    };

   public:
    void run() {
        if (!init()) throw std::runtime_error("application initialization failed");
        while (!WindowShouldClose()) {
            BeginDrawing();
            float dt = GetFrameTime();
            this->state_manager.get_current_state()->on_input();
            this->state_manager.get_current_state()->on_update(dt);
            this->state_manager.get_current_state()->on_render();
            EndDrawing();
        }

        CloseWindow();
    };

    ~application() {
        auto* current_state = this->state_manager.get_current_state();
        current_state->on_exit();
    };
};
};  // namespace loca