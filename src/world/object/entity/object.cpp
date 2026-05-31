#include "object.hpp"

#include "fmt/core.h"

namespace world {

namespace object {

namespace entity {

void
remapping_t::read_map(std::unordered_map<std::string, std::string> input) {
    for (const auto& [k, v] : input)
        color_map[std::stoull(k, nullptr, 16)] = std::stoull(v, nullptr, 16);
}

std::unordered_map<std::string, std::string>
remapping_t::write_map() const {
    std::unordered_map<std::string, std::string> res;

    for (const auto& [key, value] : color_map) {
        std::string str_key = fmtquill::format("{:08X}", key);
        std::string str_value = fmtquill::format("{:08X}", value);

        res.insert({str_key, str_value});
    }

    return res;
}

} // namespace entity

} // namespace object

} // namespace world
