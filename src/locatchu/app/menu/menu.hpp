#pragma once
#include <forge/forge.hpp>

#include "../serialize.hpp"
#include "../states.hpp"

namespace loca {

class menu {
    config::menu_config config;

   public:
    menu(const config::menu_config& config)
        : config{config} {};
    void run() {};
    ~menu() = default;
};
};  // namespace loca