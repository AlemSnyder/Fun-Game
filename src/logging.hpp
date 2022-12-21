#pragma once

#include "util/files.hpp"

#include <config.h>
#include <quill/Quill.h>

#include <string>

namespace logging {

#if DEBUG()
static constexpr quill::LogLevel DEFAULT_LOG_LEVEL = quill::LogLevel::Debug;
#else
static constexpr quill::LogLevel DEFAULT_LOG_LEVEL = quill::LogLevel::Info;
#endif

extern quill::LogLevel _LOG_LEVEL;

extern quill::Logger* opengl_logger;   // for glfw, glew etc
extern quill::Logger* terrain_logger;  // for terrain, chunk, tile class
extern quill::Logger* game_map_logger; // for terrain generation
extern quill::Logger* voxel_logger;    // for voxel logic like mesh creation
extern quill::Logger* file_io_logger;  // for file io
#if 0
extern quill::Logger* lua_logger;     // for lua logging
#endif

inline quill::Logger*
get_logger()
{
    return quill::get_logger();
}

inline quill::Logger*
get_logger(std::string name)
{
    quill::Logger* logger;

    try {
        logger = quill::get_logger(name.c_str());
    } catch (quill::QuillError&) {
        logger = quill::create_logger(name);
        logger->set_log_level(_LOG_LEVEL);
        logger->init_backtrace(5, quill::LogLevel::Error);
    }

    return logger;
}

inline void
set_thread_name(std::string name)
{
    quill::detail::set_thread_name(name.c_str());
}

inline std::filesystem::path
log_dir() noexcept
{
    return files::get_root_path() / "logs";
}

void init(
    bool console = true, quill::LogLevel log_level = DEFAULT_LOG_LEVEL,
    bool structured = true
);

} // namespace logging
