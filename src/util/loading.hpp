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
 * @file loading.hpp
 *
 * @brief Does nothing.
 *
 * @ingroup Util
 *
 */

#pragma once

#include "files.hpp"
#include "global_context.hpp"
#include "manifest.hpp"
#include "world/entity/object_handler.hpp"

#include <iostream>
#include <string>
#include <vector>

// Everything that needs to be loaded from files

namespace util {

template <bool opengl>
int
load_manifest_test() {
    auto manifest_folder = files::get_manifest_path();

    std::vector<std::future<int>> futures;

    for (const auto& directory_entry :
         std::filesystem::directory_iterator(manifest_folder)) {
        auto manifest_opt =
            files::read_json_from_file<manifest::manifest_t>(directory_entry.path());

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
            logging::file_io_logger, "Loading manifest {} with {} biomes, {} entities.",
            manifest.name, num_biomes, num_entities
        );
        if (manifest.entities) {
            // iterate through objects in manifest and queue them to be loaded
            for (const manifest::descriptor_t& entity_data : *manifest.entities) {
                // Will check if path exists

                auto future = context.submit_task([entity_data]() {
                    world::entity::ObjectHandler& object_handler =
                        world::entity::ObjectHandler::instance();
                    int result = object_handler.read_object<opengl>(entity_data);
                    return result;
                });
                futures.push_back(std::move(future));
            }
        }
        if (manifest.biomes.has_value()) {
            for (manifest::descriptor_t& biome : *manifest.biomes) {
                auto biome_data = files::read_json_from_file<terrain::generation::biome_data_t>(
                    files::get_data_path() / biome.path
                );
                if (!biome_data.has_value()) {
                    continue;
                }

                context.load_script_file(files::get_data_path() / biome.path.remove_filename() / biome_data->map_generator_path);
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

// load manifest to object handler
// This loads objects from qb files onto the gpu
inline void
load_manifest() {
    load_manifest_test<true>();
}

} // namespace util
