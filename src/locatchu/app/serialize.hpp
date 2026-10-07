#pragma once
#include <raylib.h>

#include <nlohmann/json.hpp>
namespace nlohmann {
template <>
struct adl_serializer<Color> {
    static void to_json(json& j, const Color& c) {
        j = json{
            {"r", c.r},
            {"g", c.g},
            {"b", c.b},
            {"a", c.a}};
    }

    static void from_json(const json& j, Color& c) {
        j.at("r").get_to(c.r);
        j.at("g").get_to(c.g);
        j.at("b").get_to(c.b);
        j.at("a").get_to(c.a);
    }
};
}  // namespace nlohmann

namespace config {
struct player_config {
    int player_radius{0};
    Color player_color{BLACK};
    float player_speed{0};

    friend void to_json(nlohmann::json& j, const player_config& c) {
        j = {
            {"player_radius", c.player_radius},
            {"player_color", {{"r", c.player_color.r}, {"g", c.player_color.g}, {"b", c.player_color.b}, {"a", c.player_color.a}}},
            {"player_speed", c.player_speed}};
    }

    friend void from_json(const nlohmann::json& j, player_config& c) {
        j.at("player_radius").get_to(c.player_radius);

        const auto& color = j.at("player_color");
        color.at("r").get_to(c.player_color.r);
        color.at("g").get_to(c.player_color.g);
        color.at("b").get_to(c.player_color.b);
        color.at("a").get_to(c.player_color.a);

        j.at("player_speed").get_to(c.player_speed);
    }
};

};  // namespace config