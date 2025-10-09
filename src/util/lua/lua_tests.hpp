#pragma once

#include <argh.h>

namespace util {

namespace lua {

int lua_log_test();

int lua_loadtime_test();

int lua_transfertime_test();

int lua_load_tests();

int lua_object_test();

int lua_tests(const argh::parser& cmdl);
} // namespace lua

} // namespace util
