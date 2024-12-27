#pragma once

#include "object.hpp"
#include "position_synchronizer.hpp"


#include <atomic>
#include <memory>
#include <thread>
#include <unordered_map>

namespace world {

namespace entity {

class PositionSynchronizer {
 private:
    //    std::thread thread_;

    std::atomic<bool> execute_again_ = false;

    std::future<void> result_;

 public:
    inline PositionSynchronizer() {};

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
