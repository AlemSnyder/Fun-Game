// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software
 *
 * Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/**
 * @file global_context.hpp
 *
 * @brief Defines Global Context class
 *
 * @ingroup --
 */

#pragma once

#include "logging.hpp"

#define BS_THREAD_POOL_ENABLE_PRIORITY
#include <BS_thread_pool.hpp>
#include <sol/sol.hpp>

#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <set>
#include <thread>
#include <unordered_map>

/**
 * @brief Any global context that are needed will go in this class.
 *
 * @details In particular this will contain the thread pool.
 */

class GlobalContext {
 private:
    BS::thread_pool thread_pool_;

    // opengl call backs must be run on main thread. Add them to this queue
    // then run them on main thread.
    std::queue<std::function<void()>> opengl_functions;

    std::mutex opengl_queue_mutex;

    sol::state lua_;

#if DEBUG()

    std::thread::id main_thread_id;

#endif

    // Private CTOR as this is a singleton
    GlobalContext();

 public:
    // Delete all CTORs and CTOR-like operators
    GlobalContext(GlobalContext&&) = delete;
    GlobalContext(GlobalContext const&) = delete;

    void operator=(GlobalContext&&) = delete;
    void operator=(GlobalContext const&) = delete;

    inline void
    set_main_thread() {
#if DEBUG()
        assert(
            main_thread_id == std::thread::id()
            && "Cannot set main thread id if it is already initialized."
        );
        main_thread_id = std::this_thread::get_id();
#endif
    }

    inline bool
    is_main_thread() const {
#if DEBUG()
        return main_thread_id == std::this_thread::get_id();
#else
        return true;
#endif
    }

    // Instance accessor
    static inline GlobalContext&
    instance() {
        static GlobalContext obj;
        return obj;
    }

    void run_opengl_queue();

    /**
     * @brief push task to opengl
     */
    void
    push_opengl_task(std::function<void()> task) {
        std::lock_guard<std::mutex> lock(opengl_queue_mutex);
        opengl_functions.push(task);
    }

    /**
     * @brief Initializes the threads
     *
     * Not defined, and still compiles/runs will add it if we want to specify
     * the number of threads.
     */
    // void start_threads();

    /**
     * @brief submit task to thread pool
     *
     * @param F&& function to run
     * @param BS::priority_t priority = BS::pr::normal
     */
    template <typename F, typename R = std::invoke_result_t<std::decay_t<F>>>
    auto
    submit_task(F&& function, BS::priority_t priority = BS::pr::normal) {
        return thread_pool_.submit_task(function, priority);
    }

    /**
     * @brief push task to thread pool
     */
    template <class F>
    void
    push_task(F&& function, BS::priority_t priority = BS::pr::normal) {
        thread_pool_.detach_task(function, priority);
    }

    // Might want to expose these in the future.
    auto
    wait_for_tasks() {
        return thread_pool_.wait();
    }

    // oh boy time to start wrapping tread_pool

    void load_script_file(const std::filesystem::path& path);

    std::optional<sol::object> get_from_lua(const std::string& command);
};
