#pragma once

#include <string>

namespace as_logging {

inline void
as_log_backtrace(std::string message);

inline void
as_log_info(std::string message);

inline void
as_log_debug(std::string message);

inline void
as_log_warning(std::string message);

inline void
as_log_error(std::string message);

inline void
as_log_critical(std::string message);

} // namespace name

