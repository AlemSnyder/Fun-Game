
#include "loading.hpp"

#include "global_context.hpp"
#include "manifest.hpp"
#include "world/entity/object_handler.hpp"

#include <iostream>
#include <string>

namespace util {

void
load_manifest() {
    auto manifest_folder = files::get_manifest_path();

    for (const auto& directory_entry :
         std::filesystem::directory_iterator(manifest_folder)) {
        Manifest::manifest_t manifest;

        // path to manifest file
        auto manifest_file = directory_entry.path();

        auto contents = files::open_data_file(manifest_file);
        if (contents.has_value()) [[likely]] {
            // might fail
            // if the json has some problems this will throw
            std::string content(
                (std::istreambuf_iterator<char>(contents.value())),
                std::istreambuf_iterator<char>()
            );
            auto ec = glz::read_json(manifest, content);
            if (ec) {
                LOG_ERROR(
                    logging::file_io_logger, "{}", glz::format_error(ec, content)
                );
                continue;
            }

        } else {
            // Warning is already logged when opening file
            // just move onto next file
            continue;
        }

        // TODO @AlemSnyder log a bunch of information about the manifest
        // this information doesn't exist yet, but when it does

        int num_biomes =
            manifest.biomes.has_value() ? manifest.biomes.value().size() : 0;
        int num_entities =
            manifest.entities.has_value() ? manifest.entities.value().size() : 0;

        LOG_DEBUG(
            logging::file_io_logger, "Loading manifest {} with {} biomes, {} entities.",
            manifest.name, num_biomes, num_entities
        )

        // iterate through objects in manifest and queue them to be loaded
        for (const Manifest::entity_t& entity_data : manifest.entities) {
            // Will check if path exists
            GlobalContext& context = GlobalContext::instance();
            auto future = context.submit([entity_data]() {
                world::entity::ObjectHandler& object_handler =
                    world::entity::ObjectHandler::instance();
                object_handler.read_object(entity_data);
            });
        }
    }
}

} // namespace util
