#include "keymapping.hpp"

#include "logging.hpp"

#include <unordered_set>

namespace gui {

namespace scene {

// TODO
KeyMapping::KeyMapping() : key_mapping_map_() {}

KeyMapping::KeyMapping(std::unordered_map<Action, Key> key_mapping_map) : KeyMapping() {
    // all keys must be unique
    // product of map architecture

    // all actions must be unique

    auto temp_map = key_mapping_map_;

    // all actions must be covered

    std::erase_if(temp_map, [&key_mapping_map](const auto& item) {
        return key_mapping_map.contains(item.second);
    });

    bool remapping_ok = true;

    for (const auto& [a, k] : key_mapping_map) {
        if (temp_map.contains(k)) {
            // somehow I have decided this is bad
            LOG_ERROR(
                logging::main_logger,
                "Remapping \"{}\" to \"{}\" leaves \"{}\" without a key.", to_string(k),
                to_string(a), to_string(key_mapping_map_.at(k))
            );
            remapping_ok = false;
        }
        temp_map.insert_or_assign(k, a);
    }

    if (remapping_ok) {
        key_mapping_map_ = temp_map;
    } else {
        LOG_ERROR(logging::main_logger, "Remapping not set because of errors.");
    }
}

Action
KeyMapping::operator[](Key key) const {
    const auto& iterator = key_mapping_map_.find(key);
    if (iterator != key_mapping_map_.end()) {
        return iterator->second;
    } else {
        return Action::__NO_ACTION__;
    }
}

} // namespace scene

} // namespace gui
