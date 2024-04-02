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

#include <BS_thread_pool.hpp>

#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <set>

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

    // Private CTOR as this is a singleton
    GlobalContext() //:
    // TODO update to thread pool 4.0
    //        thread_pool_([] { quill::detail::set_thread_name("BS thread pool"); })
    {}

 public:
    // Delete all CTORs and CTOR-like operators
    GlobalContext(GlobalContext&&) = delete;
    GlobalContext(GlobalContext const&) = delete;

    void operator=(GlobalContext&&) = delete;
    void operator=(GlobalContext const&) = delete;

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
        return opengl_functions.push(task);
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
     */
    template <class... Args>
    auto
    submit(Args&&... args) {
        return thread_pool_.submit(std::forward<Args>(args)...);
    }

    /**
     * @brief push task to thread pool
     */
    template <class... Args>
    void
    push_task(Args&&... args) {
        return thread_pool_.push_task(std::forward<Args>(args)...);
    }

    auto
    wait_for_tasks() {
        return thread_pool_.wait_for_tasks();
    }

    // oh boy time to start wrapping tread_pool
};
