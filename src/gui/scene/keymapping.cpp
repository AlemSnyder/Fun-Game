#include "keymapping.hpp"

#include "logging.hpp"

#include <unordered_set>

namespace gui {

namespace scene {

KeyMapping::KeyMapping() : key_mapping_map_() {
    // TODO redo this when interent
    key_mapping_map_.insert_or_assign(Action::FORWARD, Key::W);
    key_mapping_map_.insert_or_assign(Action::LEFT, Key::A);
    key_mapping_map_.insert_or_assign(Action::BACKWARD, Key::S);
    key_mapping_map_.insert_or_assign(Action::RIGHT, Key::D);
    key_mapping_map_.insert_or_assign(Action::PRIMARY_INTERACT, Key::MOUSE_LEFT);
    key_mapping_map_.insert_or_assign(Action::SECONDARY_INTERACT, Key::MOUSE_RIGHT);
    key_mapping_map_.insert_or_assign(Action::UP, Key::SPACE);
    key_mapping_map_.insert_or_assign(Action::FAST, Key::LEFT_SHIFT);
}

KeyMapping::KeyMapping(std::unordered_map<Action, Key> key_mapping_map) : KeyMapping() {
    // all keys must be unique
    // product of map architecture

    // all actions must be unique

    auto temp_map = key_mapping_map_;

    // all actions must be covered
    // all actions are already covered
    for (const auto& [a, k] : key_mapping_map) {
        temp_map.insert_or_assign(a, k);
    }

    bool remapping_ok = true;

    std::unordered_map<Key, Action> keys_mapped;
    for (const auto& [a, k] : temp_map) {
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

    if (remapping_ok) {
        key_mapping_map_ = temp_map;
    } else {
        LOG_ERROR(logging::main_logger, "Remapping not set because of errors.");
    }
}

Key
KeyMapping::operator[](Action action) const {
    const auto& iterator = key_mapping_map_.find(action);
    if (iterator != key_mapping_map_.end()) {
        return iterator->second;
    } else {
        return Key::NOT_BOUND;
    }
}

} // namespace scene

} // namespace gui
