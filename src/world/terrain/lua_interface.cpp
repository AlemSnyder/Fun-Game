#include "lua_interface.hpp"

namespace world {

namespace terrain {

void init_lua_state(sol::state& lua){
    lua.new_usertype<PathFinder>(
        "find", &PathFinder
    )

    lua.new_usertype<terrain>


}

} // namespace terrain

}
