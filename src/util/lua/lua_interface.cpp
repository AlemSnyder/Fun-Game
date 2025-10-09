// Anything in util should go in here
#include "lua_interface.hpp"

#include "types.hpp"

namespace util {

namespace lua {

void
init_lua_interface(sol::state& lua) {
    // glm::vec3
    {
        lua.new_usertype<glm::vec3>(
            "vec3", sol::meta_function::construct,
            sol::factories(
                // FractalNoise.new(...) -- dot syntax, no "self" value
                // passed in
                [](float x, float y, float z) {
                    return std::make_shared<glm::vec3>(x, y, z);
                },
                // FractalNoise:new(...) -- colon syntax, passes in the
                // "self" value as first argument implicitly
                [](sol::object, float x, float y, float z) {
                    return std::make_shared<glm::vec3>(x, y, z);
                }
            ),
            // FractalNoise(...) syntax, only
            sol::call_constructor, sol::factories([](float x, float y, float z) {
                return std::make_shared<glm::vec3>(x, y, z);
            }),
            "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z
        );
    }
}

} // namespace lua

} // namespace util
