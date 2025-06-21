// local context
// for local lua, and anything else that is local
#pragma once

// #include "logging.hpp"
// #include "global_context.hpp"
#include <sol/sol.hpp>
// #include <thread>

class GlobalCnotext;

class LocalContext {
 private:
    LocalContext();
    sol::state lua_state;

 public:
    static LocalContext& get_local_context();
    static sol::state& get_lua_state();

    friend class GlobalContext;
};
