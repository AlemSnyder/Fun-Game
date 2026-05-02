#include "keymapping.hpp"

#include "logging.hpp"

#include <unordered_set>

namespace gui {

namespace scene {

KeyMapping::KeyMapping() {
    array_map_[Action::FORWARD] = Key::W;
    array_map_[Action::LEFT] = Key::A;
    array_map_[Action::BACKWARD] = Key::S;
    array_map_[Action::RIGHT] = Key::D;
    array_map_[Action::PRIMARY_INTERACT] = Key::MOUSE_LEFT;
    array_map_[Action::SECONDARY_INTERACT] = Key::MOUSE_RIGHT;
    array_map_[Action::UP] = Key::SPACE;
    array_map_[Action::FAST] = Key::LEFT_SHIFT;
}

KeyMapping::KeyMapping(std::unordered_map<Action, Key> key_mapping_map) : KeyMapping() {
    // all keys must be unique
    // I think we test for this

    // all actions must be unique
    // all actions must be covered
    // product of fixed length array
    auto temp_map = array_map_;

    // just write keybindings into the array. well test if they are ok later
    for (const auto& [a, k] : key_mapping_map) {
        temp_map[a] = k;
    }

    // test if the new state is ok
    bool remapping_ok = true;
    std::unordered_map<Key, Action> keys_mapped;
    for (Action a = Action::FORWARD; a < Action::__NO_ACTION__;
         a = static_cast<Action>(a + 1)) {
        const auto k = temp_map[a];
        auto result = keys_mapped.insert(std::make_pair(k, a));
        if (!result.second) {
            LOG_ERROR(
                logging::main_logger,
                "Remapping \"{}\" to \"{}\" key already mapped to {}. (The error is "
                "real the log maybe not so much...)",
                to_string(k), to_string(a), to_string(keys_mapped[k])
            );
            remapping_ok = false;
        }
    }

    if (remapping_ok) { // the new state is ok
        array_map_ = temp_map;
    } else { // the new state is not ok (do not change key map)
        LOG_ERROR(logging::main_logger, "Remapping not set because of errors.");
    }
}

Key
KeyMapping::operator[](Action action) const {
    if (action < Action::__NO_ACTION__) {
        return array_map_[action];
    } else {
        return Key::NOT_BOUND;
    }
}

} // namespace scene

} // namespace gui
