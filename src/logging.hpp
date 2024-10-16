#pragma once

#include "util/files.hpp"

#include <config.h>
#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/Logger.h>
#include <quill/LogMacros.h>
#include <quill/sinks/FileSink.h>
#include <quill/std/FilesystemPath.h>
#include <quill/std/Map.h>
#include <quill/std/Pair.h>
#include <quill/std/Set.h>
#include <quill/std/Vector.h>

#include <string>

namespace logging {

#if DEBUG()
static constexpr quill::LogLevel DEFAULT_LOG_LEVEL = quill::LogLevel::Debug;
#else
static constexpr quill::LogLevel DEFAULT_LOG_LEVEL = quill::LogLevel::Info;
#endif

extern quill::LogLevel _LOG_LEVEL;

extern quill::Logger* main_logger;     // for general logging
extern quill::Logger* opengl_logger;   // for glfw, glew etc
extern quill::Logger* terrain_logger;  // for terrain, chunk, tile class
extern quill::Logger* game_map_logger; // for terrain generation
extern quill::Logger* voxel_logger;    // for voxel logic like mesh creation
extern quill::Logger* file_io_logger;  // for file io
extern quill::Logger* lua_logger;      // for lua logging

inline void
set_thread_name(std::string name) {
    quill::detail::set_thread_name(name.c_str());
}

inline std::filesystem::path
log_dir() noexcept {
    return files::get_root_path() / "logs";
}

void init(bool console = true, quill::LogLevel log_level = DEFAULT_LOG_LEVEL);

} // namespace logging
