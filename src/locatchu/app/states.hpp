#pragma once
#include <memory>
#include <vector>
namespace loca {
class state_interface {
   public:
    virtual void on_update(float) = 0;
    virtual void on_render() = 0;
    virtual void on_init() = 0;
    virtual void on_exit() = 0;
    virtual void on_input() = 0;
    virtual ~state_interface() = default;
};

template <typename T>
concept stateful = requires(T& t) {
    { t.on_init() };
    { t.on_exit() };
};

template <typename IdType, stateful StateType>
    requires(std::is_enum_v<IdType>)
class state_manager {
    std::unordered_map<IdType, std::unique_ptr<StateType>> state_map;
    StateType* current_state{nullptr};

   public:
    template <typename T, typename... Args>
        requires(std::derived_from<T, StateType> && std::constructible_from<T, Args...>)
    void register_state(IdType id, Args&&... args) {
        if (state_map.contains(id)) return;
        auto state = std::make_unique<T>(std::forward<Args>(args)...);
        this->state_map[id] = std::move(state);
    };

    StateType* get_state(IdType id) {
        if (!state_map.contains(id)) return nullptr;
        return this->state_map[id].get();
    };

    StateType* get_current_state() {
        return this->current_state;
    }

    void set_current_state(IdType id) {
        if (!this->state_map.contains(id) || this->state_map[id].get() == this->current_state) return;
        if (this->current_state) this->current_state->on_exit();
        this->current_state = state_map[id].get();
        this->current_state->on_init();
    };
};

}  // namespace loca