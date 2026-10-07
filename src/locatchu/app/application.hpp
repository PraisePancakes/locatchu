#pragma once

#include <raylib.h>

#include <fstream>
#include <nlohmann/json.hpp>

#include "game/game.hpp"
#include "serialize.hpp"
namespace loca {
class application {
    config::world_config world_config{};
    config::app_config app_config{};

    template <typename T>
        requires(config::json_compatible<T>)
    [[nodiscard]] bool initialize_configuration(const char* config_path, T& config) {
        const std::filesystem::path path = std::filesystem::path{PROJECT_ROOT} / config_path;
        std::filesystem::create_directories(path.parent_path());
        if (!std::filesystem::exists(path)) {
            std::ofstream file{path};
            if (!file.is_open()) return false;
            nlohmann::json j = config;
            file << j;
            return true;
        }
        std::ifstream file{path};
        if (!file.is_open()) return false;
        nlohmann::json data = nlohmann::json::parse(file);
        config = data.get<T>();
        return true;
    }

    [[nodiscard]] bool init() {
        return initialize_configuration("config/world_config.json", world_config) &&
               initialize_configuration("config/app_config.json", app_config);
    };

   public:
    void run() {
        if (!init()) throw std::runtime_error("application initialization failed");
        InitWindow(this->app_config.screen_width, this->app_config.screen_height, this->app_config.title.c_str());
        game{world_config}.run();
        CloseWindow();
    };
};
};  // namespace loca