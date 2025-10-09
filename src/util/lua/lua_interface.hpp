#pragma once

#include <sol/sol.hpp>

namespace util {

namespace lua {

/**
 * @brief General initializer for lua interface.
 *
 * @details Initialized types etc.
 *
 * @param lua
 */
void init_lua_interface(sol::state& lua);

} // namespace lua

} // namespace util
