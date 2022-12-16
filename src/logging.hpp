#include "util/files.hpp"

namespace logging {

inline std::filesystem::path
log_dir() noexcept {
    return files::get_root_path() / "logs";
}

void init();

} // namespace logging
