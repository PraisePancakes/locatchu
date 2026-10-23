#pragma once
#include <raylib.h>

#include <nlohmann/json.hpp>
namespace nlohmann {
template <>
struct adl_serializer<Color> {
    static void to_json(ordered_json& j, const Color& c) {
        j = ordered_json{
            {"r", c.r},
            {"g", c.g},
            {"b", c.b},
            {"a", c.a}};
    }

    static void from_json(const ordered_json& j, Color& c) {
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
    requires(nlohmann::ordered_json& j, const T& value) {
        to_json(j, value);
    };

template <typename T>
concept json_deserializable =
    requires(const nlohmann::ordered_json& j, T& value) {
        from_json(j, value);
    };

template <typename T>
concept json_compatible = json_serializable<T> && json_deserializable<T>;

template <typename T>
concept configuration_compatible = requires(const T& t) {
    { t.configuration_path } -> std::convertible_to<std::string>;
};

template <typename T>
concept is_configuration = configuration_compatible<T> && json_compatible<T>;

struct app_config {
    const char* configuration_path = "config/app_config.json";
    int screen_width{800};
    int screen_height{800};
    std::string title = "locatchu";

    friend void to_json(nlohmann::ordered_json& j, const app_config& c) {
        j = nlohmann::ordered_json{
            {"screen_width", c.screen_width},
            {"screen_height", c.screen_height},
            {"title", c.title}};
    }

    friend void from_json(const nlohmann::ordered_json& j, app_config& c) {
        j.at("screen_width").get_to(c.screen_width);
        j.at("screen_height").get_to(c.screen_height);
        j.at("title").get_to(c.title);
    }
};

struct world_config {
    const char* configuration_path = "config/world_config.json";
    float player_radius{25};
    Color player_color{BLACK};
    float player_speed{200};
    float player_running_speed_modifier{1.5};
    float enemy_radius{25};
    Color enemy_color{RED};
    float enemy_speed{100.f};
    float bullet_speed{200};
    float bullet_radius{5.f};
    Color bullet_color{BLACK};
    float bullet_lifetime{2.f};

    friend void to_json(nlohmann::ordered_json& j, const world_config& c) {
        j = nlohmann::ordered_json{
            {"player_radius", c.player_radius},
            {"player_color", c.player_color},
            {"player_speed", c.player_speed},
            {"enemy_color", c.enemy_color},
            {"enemy_radius", c.enemy_radius},
            {"player_running_speed_modifier", c.player_running_speed_modifier},
            {"bullet_speed", c.bullet_speed},
            {"bullet_radius", c.bullet_radius},
            {"bullet_color", c.bullet_color},
            {"bullet_lifetime", c.bullet_lifetime},
            {"enemy_speed", c.enemy_speed}

        };
    }

    friend void from_json(const nlohmann::ordered_json& j, world_config& c) {
        j.at("player_radius").get_to(c.player_radius);
        j.at("player_color").get_to(c.player_color);
        j.at("player_speed").get_to(c.player_speed);
        j.at("enemy_radius").get_to(c.enemy_radius);
        j.at("enemy_color").get_to(c.enemy_color);
        j.at("player_running_speed_modifier").get_to(c.player_running_speed_modifier);
        j.at("bullet_speed").get_to(c.bullet_speed);
        j.at("bullet_radius").get_to(c.bullet_radius);
        j.at("bullet_color").get_to(c.bullet_color);
        j.at("bullet_lifetime").get_to(c.bullet_lifetime);
        j.at("enemy_speed").get_to(c.enemy_speed);
    }
};

struct menu_config {
    const char* configuration_path = "config/menu_config.json";
    Color title_color{GREEN};
    int title_font_size{50};
    Color description_color{BLACK};
    int description_font_size{25};
    Color background_color{RAYWHITE};
    Color start_color{GREEN};
    int start_font_size{25};
    Color quit_color{BLACK};
    int quit_font_size{25};
    std::string title = "locatchu";
    std::string description = "An entity with no thoughts";

    friend void to_json(nlohmann::ordered_json& j, const menu_config& c) {
        j = nlohmann::ordered_json{
            {"title_color", c.title_color},
            {"title_font_size", c.title_font_size},
            {"description_color", c.description_color},
            {"description_font_size", c.description_font_size},
            {"background_color", c.background_color},
            {"start_color", c.start_color},
            {"start_font_size", c.start_font_size},
            {"quit_font_size", c.quit_font_size},
            {"quit_color", c.quit_color},
            {"title", c.title},
            {"description", c.description}

        };
    };
    friend void from_json(const nlohmann::ordered_json& j, menu_config& c) {
        j.at("title_color").get_to(c.title_color);
        j.at("title_font_size").get_to(c.title_font_size);
        j.at("description_color").get_to(c.description_color);
        j.at("description_font_size").get_to(c.description_font_size);
        j.at("background_color").get_to(c.background_color);
        j.at("start_color").get_to(c.start_color);
        j.at("start_font_size").get_to(c.start_font_size);
        j.at("quit_font_size").get_to(c.quit_font_size);
        j.at("quit_color").get_to(c.quit_color);
        j.at("title").get_to(c.title);
        j.at("description").get_to(c.description);
    }
};

};  // namespace config