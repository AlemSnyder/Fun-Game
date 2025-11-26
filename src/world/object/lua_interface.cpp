#include "lua_interface.hpp"

#include "entity/entity.hpp"
#include "entity/object.hpp"
#include "entity/plan.hpp"
#include "entity/tile_object.hpp"
#include "world/terrain/path/path.hpp"

namespace world {

namespace object {

void
init_lua_interface(sol::state& lua) {
    sol::usertype<entity::ObjectInstance> object =
        lua.new_usertype<entity::ObjectInstance>(
            "ObjectInstance", "take_damage", &entity::ObjectInstance::take_damage,
            "get_position", &entity::ObjectInstance::get_position, "get_terrain",
            &entity::ObjectInstance::get_terrain
        );

    object["get_health"] = &entity::ObjectInstance::get_health;

    lua.new_usertype<entity::TileObjectInstance>(
        "TileObjectInstance", "get_health", &entity::TileObjectInstance::get_health,
        "take_damage", &entity::TileObjectInstance::take_damage, "get_position",
        &entity::TileObjectInstance::get_position, "get_terrain",
        &entity::TileObjectInstance::get_terrain
    );

    lua.new_usertype<entity::EntityInstance>(
        "EntityInstance", "get_health", &entity::EntityInstance::get_health,
        "take_damage", &entity::EntityInstance::take_damage, "get_position",
        &entity::EntityInstance::get_position, "get_terrain",
        &entity::EntityInstance::get_terrain
    );

    //    lua.new_usertype<entity::PathTask>("PathTask");

    //    lua.new_usertype<entity::InteractTask>("InteractTask");

    lua.new_usertype<entity::Task>(
        "Task", sol::meta_function::construct,
        sol::factories(
            // Task.new(...) -- dot syntax, no "self" value
            // passed in
            [](terrain::path::Path path) {
                return std::make_shared<entity::Task>(path);
            },
            // Task:new(...) -- colon syntax, passes in the
            // "self" value as first argument implicitly
            [](sol::object, terrain::path::Path path) {
                return std::make_shared<entity::Task>(path);
            }
        ),
        // Task(...) syntax, only
        sol::call_constructor, sol::factories([](terrain::path::Path path) {
            return std::make_shared<entity::Task>(path);
        }),
        sol::meta_function::construct,
        sol::factories(
            // Task.new(...) -- dot syntax, no "self" value
            // passed in
            [](std::shared_ptr<entity::ObjectInstance> object, std::string function_name
            ) { return std::make_shared<entity::Task>(object, function_name); },
            // Task:new(...) -- colon syntax, passes in the
            // "self" value as first argument implicitly
            [](sol::object, std::shared_ptr<entity::ObjectInstance> object,
               std::string function_name) {
                return std::make_shared<entity::Task>(object, function_name);
            }
        ),
        // Task(...) syntax, only
        sol::call_constructor,
        sol::factories([](std::shared_ptr<entity::ObjectInstance> object,
                          std::string function_name) {
            return std::make_shared<entity::Task>(object, function_name);
        }),
        "can_do_task", &entity::Task::can_do_task, "do_task", &entity::Task::do_task
    );

    lua.new_usertype<entity::Plan>(
        "Plan", sol::meta_function::construct,
        sol::factories(
            // Plan.new(...) -- dot syntax, no "self" value
            // passed in
            []() { return std::make_shared<entity::Plan>(); },
            // Plan:new(...) -- colon syntax, passes in the
            // "self" value as first argument implicitly
            [](sol::object) { return std::make_shared<entity::Plan>(); }
        ),
        // Plan(...) syntax, only
        sol::call_constructor,
        sol::factories([]() { return std::make_shared<entity::Plan>(); }), "completed",
        &entity::Plan::completed, "add_task", &entity::Plan::add_task
    );
}

} // namespace object

} // namespace world
