#include "global_context.hpp"

#include "logging.hpp"

void
GlobalContext::run_opengl_queue() {
    std::lock_guard<std::mutex> lock(opengl_queue_mutex);
    if (!opengl_functions.empty()) [[unlikely]] {
        LOG_DEBUG(
            logging::opengl_logger, "Running {} functions from opengl function pool.",
            opengl_functions.size()
        );

        while (!opengl_functions.empty()) {
            const auto& function = opengl_functions.front();
            function(); // run function
            opengl_functions.pop();
        }
    }
}
