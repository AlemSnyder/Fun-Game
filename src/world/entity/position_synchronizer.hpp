#pragma once

#include "object.hpp"
#include "position_synchronizer.hpp"

#include <atomic>
#include <memory>
#include <thread>
#include <unordered_map>

namespace world {

namespace entity {

// TODO convert to background task
// I don't really like how this works.
// 1) probably shouldn't use the thread pool especially because there is much waiting
// 2) what happens to the result when the thread pool is destroyed?
// 3) objects might change want to pass the object handler or place all this logic in object handler

class PositionSynchronizer {
 private:
    //    std::thread thread_;

    std::atomic<bool> execute_again_ = false;

    std::future<void> result_;

 public:
    inline PositionSynchronizer(){};

    inline ~PositionSynchronizer() {
        if (execute_again_) {
            execute_again_ = false;
            result_.wait();
        }
    }

    void start(std::unordered_map<std::string, std::shared_ptr<Object>>& objects);

    void stop();
};

} // namespace entity

} // namespace world
