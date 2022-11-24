#pragma once

#include "config.h"

#include <string_view>

// Data and resource paths
#if DEBUG()
constexpr std::string_view ROOT_DIR_PATH = "..";
#else
constexpr std::string_view ROOT_DIR_PATH = ".";
#endif
