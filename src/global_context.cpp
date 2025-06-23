#include "global_context.hpp"

#include "local_context.hpp"
#include "logging.hpp"

void
GlobalContext::run_opengl_queue() {
    std::lock_guard<std::mutex> lock(opengl_queue_mutex);
    if (!opengl_functions.empty()) [[unlikely]] {
#if 0
        LOG_DEBUG(
            logging::opengl_logger, "Running {} functions from opengl function pool.",
            opengl_functions.size()
        );
#endif

        while (!opengl_functions.empty()) {
            const auto& function = opengl_functions.front();
            function(); // run function
            opengl_functions.pop();
        }
        LOG_DEBUG_LIMIT(
            std::chrono::seconds{60}, logging::opengl_logger, "Completed opengl queue."
        );
    }
}

GlobalContext::GlobalContext() :
    thread_pool_([] { quill::detail::set_thread_name("BS Thread"); }) {
    std::mutex local_context_map_mut;
    auto future = submit_task([this, &local_context_map_mut]() {
        auto local_context = LocalContext();
        std::scoped_lock lock(local_context_map_mut);
        local_thread_contexts.insert(
            {std::this_thread::get_id(), std::move(local_context)}
        );
    });
    auto local_context = LocalContext();
    {
        std::scoped_lock lock(local_context_map_mut);
        local_thread_contexts.insert({std::this_thread::get_id(), std::move(local_context)});
    }

    future.wait();
}
