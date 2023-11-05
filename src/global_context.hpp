#pragma once

#include "BS_thread_pool.hpp"
#include "entity/mesh.hpp"
#include "terrain/chunk.hpp"

#include <map>
#include <mutex>
#include <set>

/**
 * @file
 */

class GlobalContext {
 private:
    BS::thread_pool thread_pool_;

    // Private CTOR as this is a singleton
    GlobalContext() {}

 public:
    // Delete all CTORs and CTOR-like operators
    GlobalContext(GlobalContext&&) = delete;
    GlobalContext(GlobalContext const&) = delete;

    void operator=(GlobalContext&&) = delete;
    void operator=(GlobalContext const&) = delete;

    // Instance accessor
    static inline GlobalContext&
    getInstance() {
        static GlobalContext obj;
        return obj;
    }

    void start();

    template <class... Args>
    auto
    submit(Args&&... args) {
        return thread_pool_.submit(std::forward<Args>(args)...);
    }

    template <class... Args>
    auto
    push_task(Args&&... args) {
        return thread_pool_.push_task(std::forward<Args>(args)...);
    }

    auto
    wait_for_tasks() {
        return thread_pool_.wait_for_tasks();
    }

    // oh boy time to start wrapping tread_pool
};
