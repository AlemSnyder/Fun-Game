#include "position_synchronizer.hpp"

#include <chrono>

// TODO this should be set dynamically by the framerate
constexpr std::chrono::milliseconds sleep_time(200);

namespace world {

namespace entity {

void
PositionSynchronizer::start(
    std::unordered_map<std::string, std::shared_ptr<Object>>& objects
) {
    execute_again_ = true;

    GlobalContext& context = GlobalContext::instance();

    result_ = context.submit_task([this, &objects]() {
        while (execute_again_) {
            auto now = std::chrono::system_clock::now();

            for (auto& [name, object] : objects) {
                object->update();
            }
            // time waiting

            std::this_thread::sleep_until(now + sleep_time);
        }
    });
}

void
PositionSynchronizer::stop() {
    execute_again_ = false;
};

} // namespace entity

} // namespace world
