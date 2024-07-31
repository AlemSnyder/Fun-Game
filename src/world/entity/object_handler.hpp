#include "model.hpp"

#include <filesystem>
#include <map>
#include <mutex>
#include <string>

namespace world {

namespace entity {

/**
 * @brief Handles all objects
 *
 * @details Global set of objects for look up. All objects should be read from files
 * using this handler so that any system can look up them later by the object id string.
 */
class ObjectHandler {
 protected:
    // Private CTOR as this is a singleton
    ObjectHandler() {}

    std::mutex map_mutex_;
    std::map<std::string, ObjectData> ided_objects;

 public:
    // Delete all CTORs and CTOR-like operators
    ObjectHandler(ObjectHandler&&) = delete;
    ObjectHandler(ObjectHandler const&) = default;

    ObjectHandler& operator=(ObjectHandler&&) = delete;
    ObjectHandler& operator=(ObjectHandler const&) = default;

    // Instance accessor
    static inline ObjectHandler&
    instance() {
        static ObjectHandler obj;
        return obj;
    }

    void read_object(std::filesystem::path);

    ObjectData& get_object(const std::string&);

    [[nodiscard]] inline std::map<std::string, ObjectData>&
    get_objects() {
        return ided_objects;
    };

    /**
     * @brief Update all ObjectData. Should be run once per frame.
     */
    void update();
};

} // namespace entity

} // namespace world
