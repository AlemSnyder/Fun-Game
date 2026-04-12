#pragma once

#include <angelscript.h>
#include <sol/sol.hpp>

namespace terrain {

namespace generation {

void init_lua_interface(sol::state& lua);

void init_as_interface(asIScriptEngine* engine);

} // namespace generation

} // namespace terrain
