#include "lua_interface.hpp"

#include "entity/entity.hpp"
#include "entity/object.hpp"
#include "entity/tile_object.hpp"

namespace world {

namespace object {

void
init_lua_interface(sol::state& lua) {
    sol::usertype<entity::ObjectInstance> object =
        lua.new_usertype<entity::ObjectInstance>(
            "ObjectInstance", "take_damage", &entity::ObjectInstance::take_damage,
            "get_position", &entity::ObjectInstance::get_position
        );

    object["get_health"] = &entity::ObjectInstance::get_health;

    lua.new_usertype<entity::TileObjectInstance>(
        "TileObjectInstance", "get_health", &entity::TileObjectInstance::get_health,
        "take_damage", &entity::TileObjectInstance::take_damage, "get_position",
        &entity::TileObjectInstance::get_position
    );

    lua.new_usertype<entity::EntityInstance>(
        "EntityInstance", "get_health", &entity::EntityInstance::get_health,
        "take_damage", &entity::EntityInstance::take_damage, "get_position",
        &entity::EntityInstance::get_position
    );
}

} // namespace object

} // namespace world
