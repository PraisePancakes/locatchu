#pragma once
#include <memory>
#include <vector>
namespace loca {
enum class APP_STATES : std::uint8_t {
    GAME,
    MENU,
    QUIT
};

template <typename T>
    requires(std::is_enum_v<T>)
class state_interface {
    using transition_callback = std::function<void(T)>;
    transition_callback _transition;

   public:
    using ID_TYPE = T;
    state_interface(transition_callback t) : _transition{std::move(t)} {};
    virtual void on_update(float) = 0;
    virtual void on_render() = 0;
    virtual void on_init() = 0;
    virtual void on_exit() = 0;
    virtual void on_input() = 0;
    virtual ~state_interface() = default;

   protected:
    void request_transition(T t) {
        _transition(t);
    };
};

template <typename T>
concept stateful = requires(T& t) {
    { t.on_init() } -> std::same_as<void>;
    { t.on_exit() } -> std::same_as<void>;
};

template <stateful StateType>
class state_manager {
    using IdType = typename StateType::ID_TYPE;
    std::unordered_map<IdType, std::unique_ptr<StateType>> state_map;
    StateType* current_state{nullptr};
    IdType id;

   public:
    template <typename T, typename... Args>
        requires(std::derived_from<T, StateType> && std::constructible_from<T, Args...>)
    void register_state(IdType id, Args&&... args) {
        if (state_map.contains(id)) return;
        auto state = std::make_unique<T>(std::forward<Args>(args)...);
        this->state_map[id] = std::move(state);
    };

    void request_transition(IdType id) {
        this->id = id;
        set_current_state(id);
    }

    IdType get_current_id() const {
        return this->id;
    }

    StateType* get_state(IdType id) {
        if (!state_map.contains(id)) return nullptr;
        return this->state_map[id].get();
    };

    StateType* get_current_state() {
        return this->current_state;
    }

    void set_current_state(IdType id) {
        if (!this->state_map.contains(id) || this->state_map[id].get() == this->current_state) return;
        this->id = id;
        if (this->current_state) this->current_state->on_exit();
        this->current_state = state_map[id].get();
        this->current_state->on_init();
    };
};

}  // namespace loca