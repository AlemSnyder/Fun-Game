
#include "loading.hpp"

#include "files.hpp"
#include "global_context.hpp"
#include "manifest.hpp"
#include "world/entity/model.hpp"
#include "world/entity/object_handler.hpp"

#include <iostream>
#include <string>

namespace util {

void
load_manifest() {
    auto manifest_folder = files::get_manifest_path();

    for (const auto& directory_entry :
         std::filesystem::directory_iterator(manifest_folder)) {
        auto manifest_opt =
            files::read_json_from_file<manifest::manifest_t>(directory_entry.path());

        if (!manifest_opt) {
            continue;
        }

        manifest::manifest_t& manifest = *manifest_opt;

        int num_biomes = manifest.biomes ? (*manifest.biomes).size() : 0;
        int num_entities = manifest.entities ? (*manifest.entities).size() : 0;

        LOG_DEBUG(
            logging::file_io_logger, "Loading manifest {} with {} biomes, {} entities.",
            manifest.name, num_biomes, num_entities
        );
        if (manifest.entities) {
            // iterate through objects in manifest and queue them to be loaded
            for (const manifest::descriptor_t& entity_data : *manifest.entities) {
                // Will check if path exists
                GlobalContext& context = GlobalContext::instance();
                auto future = context.submit([entity_data]() {
                    world::entity::ObjectHandler& object_handler =
                        world::entity::ObjectHandler::instance();
                    object_handler.read_object(entity_data);
                });
            }
        }
        // if (manifest.biomes) -> do something else
    }
}

// I know this entire things is copied. I know it's bad.
// It is like this because I don't know if I can
//  1 use opengl without a screen (GitHub)
//  2 conveniently get errors back
int
load_manifest_test() {
    auto manifest_folder = files::get_manifest_path();

    std::unordered_set<std::string> ided_objects;

    for (const auto& directory_entry :
         std::filesystem::directory_iterator(manifest_folder)) {
        auto manifest_opt =
            files::read_json_from_file<manifest::manifest_t>(directory_entry.path());

        if (!manifest_opt) {
            return 1;
        }

        manifest::manifest_t& manifest = *manifest_opt;

        int num_biomes = manifest.biomes ? (*manifest.biomes).size() : 0;
        int num_entities = manifest.entities ? (*manifest.entities).size() : 0;

        LOG_DEBUG(
            logging::file_io_logger, "Loading manifest {} with {} biomes, {} entities.",
            manifest.name, num_biomes, num_entities
        );
        if (manifest.entities) {
            // iterate through objects in manifest and queue them to be loaded
            for (const manifest::descriptor_t& entity_data : *manifest.entities) {
                // Will check if path exists

                // struct to read data into
                auto object_data = files::read_json_from_file<world::entity::object_t>(
                    files::get_data_path() / entity_data.path
                );

                // read contents from path
                auto contents = files::open_data_file(entity_data.path);
                if (!contents) {
                    LOG_ERROR(
                        logging::file_io_logger,
                        "Attempting to load {} from {} failed.",
                        entity_data.identification, entity_data.path
                    );
                    return 1;
                }

                // check identification
                std::string identification = entity_data.identification;
                if (ided_objects.find(identification) != ided_objects.end()) {
                    LOG_WARNING(
                        logging::file_io_logger,
                        "Duplicate Identification \"{}\" found.", identification
                    );
                    return 1;
                }
                ided_objects.emplace(identification);
            }
        }
        // if (manifest.biomes.has_value()) -> do something else
    }
    return 0;
}

} // namespace util
