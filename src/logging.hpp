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

inline quill::Logger*
get_logger(std::string name) {
    quill::Logger* logger;

    try {
        logger = quill::get_logger(name.c_str());
    } catch (quill::QuillError&) {
        logger = quill::create_logger(name);
        logger->set_log_level(_LOG_LEVEL);
    }

    return logger;
}

inline std::filesystem::path
log_dir() noexcept {
    return files::get_root_path() / "logs";
}

void init(quill::LogLevel log_level = DEFAULT_LOG_LEVEL);

} // namespace logging
