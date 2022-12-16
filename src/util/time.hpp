
#include <sys/timeb.h>

#include <chrono>
#include <ctime>

namespace time_util {

inline int
get_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch()
    )
        .count();
}

inline std::chrono::nanoseconds
get_time_nanoseconds(){
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::system_clock::now().time_since_epoch()
    );
}

} // namespace time_util
