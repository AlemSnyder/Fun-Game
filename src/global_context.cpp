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

void
GlobalContext::require_lua_file(
    const std::string& key, const std::filesystem::path& path, bool create_global
) {
    // run this lambda on all threads in the thread pool
    // in reality should only require the file in one lua state, then copy the generated
    // byte code to all the others this will load onto all states including the one held
    // by the main thread. yes this should be made thread safe, but so should global
    // context It will be fixed before it becomes a problem, just not in this pr
    // (probably Lua-Setup)
    for (auto& [thread_id, local_context] : local_thread_contexts_) {
        sol::state& lua = local_context.get_lua_state();
        lua.require_file(key, path.string(), create_global);
    };
}

GlobalContext::GlobalContext() :
    thread_pool_([] { quill::detail::set_thread_name("BS Thread"); }) {
    std::mutex local_context_map_mut;
    auto future = submit_task([this, &local_context_map_mut]() {
        auto local_context = LocalContext();
        std::scoped_lock lock(local_context_map_mut);
        local_thread_contexts_.insert(
            {std::this_thread::get_id(), std::move(local_context)}
        );
    });
    auto local_context = LocalContext();
    {
        std::scoped_lock lock(local_context_map_mut);
        local_thread_contexts_.insert(
            {std::this_thread::get_id(), std::move(local_context)}
        );
    }

    future.wait();
}
