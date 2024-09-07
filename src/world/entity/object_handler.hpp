// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/**
 * @file object_handler.hpp
 *
 * @brief Defines ObjectHandler class.
 *
 * @ingroup World Entity
 *
 */

#include "gui/render/structures/model.hpp"
#include "manifest.hpp"
#include "object.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

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
    std::unordered_map<std::string, std::shared_ptr<Object>> ided_objects;

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

    /**
     * @brief Load new object from path.
     */
    void read_object(const manifest::descriptor_t& object_descriptor);

    std::shared_ptr<Object> get_object(const std::string&);

    [[nodiscard]] inline const auto
    begin() const {
        return ided_objects.begin();
    };

    [[nodiscard]] inline const auto
    end() const {
        return ided_objects.end();
    };

    /**
     * @brief Update all ObjectData. Should be run once per frame.
     */
    void update();
};

} // namespace entity

} // namespace world
