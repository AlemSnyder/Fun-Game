#pragma once

#include "entity/mesh.hpp"
#include "terrain/chunk.hpp"

#include <map>
#include <set>
#include <mutex>

#include "thread-pool/include/BS_thread_pool.hpp"

/**
 * @file
 */

class GlobalContext {
 private:

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

    BS::thread_pool thread_pool_;


    //using thread_pool_::submit;

    // oh boy time to start wrapping tread_pool
};
