#include "util/files.hpp"

#include <config.h>
#include <quill/Quill.h>

namespace logging {

#if DEBUG()
static constexpr quill::LogLevel DEFAULT_LOG_LEVEL = quill::LogLevel::Debug;
#else
static constexpr quill::LogLevel DEFAULT_LOG_LEVEL = quill::LogLevel::Info;
#endif

inline std::filesystem::path
log_dir() noexcept {
    return files::get_root_path() / "logs";
}

void init(quill::LogLevel log_level = DEFAULT_LOG_LEVEL);

} // namespace logging
