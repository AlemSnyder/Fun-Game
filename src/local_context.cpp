#include "local_context.hpp"

#include "global_context.hpp"
#include "util/lua/lua_logging.hpp"

LocalContext::LocalContext(/* args */) {
    lua_state.open_libraries(sol::lib::base);
    lua_state.open_libraries(sol::lib::math);
    lua_state.open_libraries(sol::lib::string);
    lua_state.open_libraries(sol::lib::debug);
    lua_logging::setup_lua_logging(lua_state);
}

LocalContext&
LocalContext::instance() {
    static thread_local LocalContext lctx;
    return lctx;
}
