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
#pragma once

/**
 * @file object_handler.hpp
 *
 * @brief Defines ObjectHandler class.
 *
 * @ingroup World Entity
 *
 */

#pragma once

#include "entity.hpp"
#include "gui/render/structures/model.hpp"
#include "implemented_entity.hpp"
#include "manifest.hpp"
#include "object.hpp"
#include "position_synchronizer.hpp"
#include "tile_object.hpp"
#include "util/files.hpp"
#include "world/biome.hpp"

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

    PositionSynchronizer position_synchronizer_;

 public:
    // Delete all CTORs and CTOR-like operators
    ObjectHandler(ObjectHandler&&) = delete;
    ObjectHandler(ObjectHandler const&) = delete;

    ObjectHandler& operator=(ObjectHandler&&) = delete;
    ObjectHandler& operator=(ObjectHandler const&) = delete;

    // Instance accessor
    static inline ObjectHandler&
    instance() {
        static ObjectHandler obj;
        return obj;
    }

    /**
     * @brief Load new object from path.
     *
     * @param manifest::descriptor_t object description
     */
    template <bool opengl = true>
    int
    read_object(const manifest::descriptor_t& descriptor) {
        // read contents from path
        auto object_data = files::read_json_from_file<object_t>(
            files::get_data_path() / descriptor.path
        );

        if (!object_data) {
            LOG_ERROR(
                logging::file_io_logger, "Failed to load {} from {}.",
                descriptor.identification, descriptor.path
            );
            return 1;
        }

        std::lock_guard<std::mutex> lock(this->map_mutex_);

        // check identification
        std::string identification = descriptor.identification;
        if (ided_objects.find(identification) != ided_objects.end()) {
            LOG_WARNING(
                logging::file_io_logger, "Duplicate Identification \"{}\" found.",
                identification
            );
            return 1;
        }

        if constexpr (opengl) {
            switch (object_data->type) {
                case OBJECT_TYPE::TILE_OBJECT:
                    {
                        std::shared_ptr<TileObject> new_object =
                            std::make_shared<TileObject>(*object_data, descriptor);

                        // when objects are initalized data is sent to the gpu.
                        // we want to run the mesher async, but need to send the data to
                        // the gpu on the main thread
                        ided_objects[identification] =
                            static_pointer_cast<Object>(new_object);
                    }
                    break;
                case OBJECT_TYPE::ENTITY:
                    {
                        std::shared_ptr<Entity> new_object =
                            std::make_shared<Entity>(*object_data, descriptor);
                        ided_objects[identification] =
                            static_pointer_cast<Object>(new_object);
                    }
                    break;

                case OBJECT_TYPE::IMPLEMENTED_ENTITY:
                    {
                        std::shared_ptr<ImplementedEntity> new_object =
                            std::make_shared<ImplementedEntity>(*object_data, descriptor);
                        ided_objects[identification] =
                            static_pointer_cast<Object>(new_object);
                    }
                    break;

                default:
                    break;
            }
        }
        return 0;
    }

    std::shared_ptr<Object> get_object(const std::string& object_id);

    /**
     * @brief Load new biome from file
     *
     * @param manifest::descriptor_t biome description
     */
    void read_biome(const manifest::descriptor_t& biome_descriptor);

    std::shared_ptr<terrain::generation::Biome> get_biome(const std::string& biome_id);

    [[nodiscard]] inline const auto
    begin() const {
        return ided_objects.begin();
    };

    [[nodiscard]] inline const auto
    end() const {
        return ided_objects.end();
    };

    /**
     * @brief Update all Object s. Should be run once per frame.
     */
    //    void update();

    void start_update();

    void stop_update();
};

} // namespace entity

} // namespace world
