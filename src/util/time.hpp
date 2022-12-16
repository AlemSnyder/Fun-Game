
#include <chrono>

namespace time_util {

[[nodiscard]] inline std::chrono::milliseconds
get_time() noexcept {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );
}

[[nodiscard]] inline std::chrono::nanoseconds
get_time_nanoseconds() noexcept {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );
}

[[nodiscard]] inline std::size_t
epoch_millis() noexcept {
    return get_time().count();
}

[[nodiscard]] inline std::size_t
epoch_nanos() noexcept {
    return get_time_nanoseconds().count();
}

} // namespace time_util
