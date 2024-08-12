#include "global_context.hpp"

#include "logging.hpp"

void
GlobalContext::run_opengl_queue() {
    if (opengl_functions.size() > 0) [[unlikely]] {
        LOG_DEBUG(
            logging::opengl_logger, "Running {} functions from opengl function pool.",
            opengl_functions.size()
        );
        std::lock_guard<std::mutex> lock(opengl_queue_mutex);

        while (!opengl_functions.empty()) {
            const auto& function = opengl_functions.front();
            function(); // run function
            opengl_functions.pop();
        }
    }
}
