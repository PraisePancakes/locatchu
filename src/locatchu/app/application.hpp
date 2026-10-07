#pragma once

#include <fstream>
#include <nlohmann/json.hpp>

#include "serialize.hpp"
namespace loca {
class Application {
    [[nodiscard]] bool init() {
        std::ifstream config_file;
        config_file.open("../config/player_config.json");
        if (!config_file.is_open()) {
        };

        return true;
    };

   public:
};
};  // namespace loca