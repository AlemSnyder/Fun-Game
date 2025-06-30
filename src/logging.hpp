#pragma once

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
extern quill::Logger* lua_runtime_logger;      // for lua logging

inline void
set_thread_name(std::string name) {
    quill::detail::set_thread_name(name.c_str());
}

void init(bool console = true, quill::LogLevel log_level = DEFAULT_LOG_LEVEL);

// TODO Quill will not log \n character. Maybe figure this out

// to flush all log methods
// it doesn't work and I don't know why
inline void
flush() {
    main_logger->flush_log();
    opengl_logger->flush_log();
    terrain_logger->flush_log();
    game_map_logger->flush_log();
    voxel_logger->flush_log();
    file_io_logger->flush_log();
    lua_logger->flush_log();
    lua_runtime_logger->flush_log();
}

} // namespace logging
