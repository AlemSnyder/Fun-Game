#pragma once

#include <sol/sol.hpp>

namespace world {

namespace terrain {

namespace generation {

void init_lua_interface(sol::state& lua);

} // namespace generation

} // namespace terrain

} // namespace world
