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

template <typename T>
concept json_serializable =
    requires(nlohmann::json& j, const T& value) {
        to_json(j, value);
    };

template <typename T>
concept json_deserializable =
    requires(const nlohmann::json& j, T& value) {
        from_json(j, value);
    };

template <typename T>
concept json_compatible = json_serializable<T> && json_deserializable<T>;

struct app_config {
    int screen_width{800};
    int screen_height{800};
    std::string title = "locatchu";

    friend void to_json(nlohmann::json& j, const app_config& c) {
        j = {
            {"screen_width", c.screen_width},
            {"screen_height", c.screen_height},
            {"title", c.title}};
    }

    friend void from_json(const nlohmann::json& j, app_config& c) {
        j.at("screen_width").get_to(c.screen_width);
        j.at("screen_height").get_to(c.screen_height);
        j.at("title").get_to(c.title);
    }
};

struct world_config {
    int player_radius{25};
    Color player_color{BLACK};
    float player_speed{200};
    int enemy_radius{25};
    Color enemy_color{RED};

    friend void to_json(nlohmann::json& j, const world_config& c) {
        j = {
            {"player_radius", c.player_radius},
            {"player_color", c.player_color},
            {"player_speed", c.player_speed},
            {"enemy_color", c.enemy_color},
            {"enemy_radius", c.enemy_radius}

        };
    }

    friend void from_json(const nlohmann::json& j, world_config& c) {
        j.at("player_radius").get_to(c.player_radius);
        j.at("player_color").get_to(c.player_color);
        j.at("player_speed").get_to(c.player_speed);
        j.at("enemy_radius").get_to(c.enemy_radius);
        j.at("enemy_color").get_to(c.enemy_color);
    }
};

};  // namespace config