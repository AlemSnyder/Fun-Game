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

LocalContext& LocalContext::get_local_context() {
    auto thread_id = std::this_thread::get_id();
    //GlobalContext& context = GlobalContext::instance();
    return GlobalContext::get_local(thread_id);
}

sol::state& LocalContext::get_lua_state() {
    return get_local_context().lua_state;
}