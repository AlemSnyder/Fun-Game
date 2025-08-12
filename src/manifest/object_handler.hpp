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

#include "gui/render/structures/model.hpp"
#include "manifest/manifest.hpp"
#include "util/files.hpp"
#include "world/biome.hpp"
#include "world/object/entity/entity.hpp"
#include "world/object/entity/implemented_entity.hpp"
#include "world/object/entity/object.hpp"
#include "world/object/entity/tile_object.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace manifest {

/**
 * @brief Handles all objects
 *
 * @details Global set of objects for look up. All objects should be read from files
 * using this handler so that any system can look up them later by the object id string.
 */
class ObjectHandler {
 protected:
    // Private CTOR as this is a singleton

    std::mutex map_mutex_;
    std::unordered_map<std::string, std::shared_ptr<world::object::entity::Object>>
        ided_objects;

 public:
    ObjectHandler() {}

    ObjectHandler(ObjectHandler&&) = default;
    ObjectHandler(ObjectHandler const&) = default;

    ObjectHandler& operator=(ObjectHandler&&) = default;
    ObjectHandler& operator=(ObjectHandler const&) = default;

    /**
     * @brief Load new object from path.
     *
     * @param manifest::descriptor_t object description
     */
    template <bool opengl = true>
    int
    read_object(const manifest::descriptor_t& descriptor) {
        // read contents from path
        auto object_data = files::read_json_from_file<world::object::entity::object_t>(
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
                case world::object::entity::OBJECT_TYPE::TILE_OBJECT:
                    {
                        std::shared_ptr<world::object::entity::TileObject> new_object =
                            std::make_shared<world::object::entity::TileObject>(
                                *object_data, descriptor
                            );

                        // when objects are initalized data is sent to the gpu.
                        // we want to run the mesher async, but need to send the data to
                        // the gpu on the main thread
                        ided_objects[identification] =
                            static_pointer_cast<world::object::entity::Object>(
                                new_object
                            );
                    }
                    break;
                case world::object::entity::OBJECT_TYPE::ENTITY:
                    {
                        std::shared_ptr<world::object::entity::Entity> new_object =
                            std::make_shared<world::object::entity::Entity>(
                                *object_data, descriptor
                            );
                        ided_objects[identification] =
                            static_pointer_cast<world::object::entity::Object>(
                                new_object
                            );
                    }
                    break;

                case world::object::entity::OBJECT_TYPE::IMPLEMENTED_ENTITY:
                    {
                        std::shared_ptr<world::object::entity::ImplementedEntity>
                            new_object = std::make_shared<
                                world::object::entity::ImplementedEntity>(
                                *object_data, descriptor
                            );
                        ided_objects[identification] =
                            static_pointer_cast<world::object::entity::Object>(
                                new_object
                            );
                    }
                    break;

                default:
                    break;
            }
        }
        return 0;
    }

    template <bool opengl>
    int
    load_all_manifests() {
        auto manifest_folder = files::get_manifest_path();

        std::vector<std::future<int>> futures;

        for (const auto& directory_entry :
             std::filesystem::directory_iterator(manifest_folder)) {
            auto manifest_opt =
                files::read_json_from_file<manifest::manifest_t>(directory_entry.path()
                );

            if (!manifest_opt) {
                if constexpr (opengl) {
                    continue;
                } else {
                    return 1;
                }
            }

            manifest::manifest_t& manifest = *manifest_opt;

            int num_biomes = manifest.biomes ? (*manifest.biomes).size() : 0;
            int num_entities = manifest.entities ? (*manifest.entities).size() : 0;

            GlobalContext& context = GlobalContext::instance();

            LOG_DEBUG(
                logging::file_io_logger,
                "Loading manifest {} with {} biomes, {} entities.", manifest.name,
                num_biomes, num_entities
            );
            if (manifest.entities) {
                // iterate through objects in manifest and queue them to be loaded
                for (const manifest::descriptor_t& entity_data : *manifest.entities) {
                    auto future = context.submit_task([this, entity_data]() {
                        int result = read_object<opengl>(entity_data);
                        return result;
                    });
                    futures.push_back(std::move(future));
                }
            }
            if (manifest.biomes.has_value()) {
                for (manifest::descriptor_t& biome : *manifest.biomes) {
                    read_biome(biome);
                }
            }
        }
        int status = 0;
        for (auto& future : futures) {
            int value = future.get();
            if (value == 1) {
                status = 1;
            }
        }
        return status;
    }

    std::shared_ptr<world::object::entity::Object>
    get_object(const std::string& object_id);

    std::shared_ptr<const world::object::entity::Object>
    get_object(const std::string& object_id) const;

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
};

} // namespace manifest
