
#include "loading.hpp"

#include "world/entity/object_handler.hpp"

#include <json/json.h>

#include <global_context.hpp>

#include <iostream>
#include <string>

namespace util {

void
load_manifest() {
    auto manifest_folder = files::get_manifest_path();

    Json::Value manifest;
    /*for (const auto& manifest_folder_entry :
        std::filesystem::directory_iterator(manifest_folder)) {
        Json::Value mod_manifest;

        std::filesystem::path manifest_file = manifest_folder_entry;

        LOG_DEBUG(logging::file_io_logger, "Opening file {}.", manifest_file.string());

        auto contents = files::open_data_file(manifest_file);
        if (contents.has_value()) {
            contents.value() >> manifest;
        } else {
            LOG_WARNING(
                logging::file_io_logger, "Cannot open file {}.", manifest_file.string()
            );
        }

        update_json(manifest, mod_manifest);
    }*/

    auto manifest_file = manifest_folder / "manifest.json";

    auto contents = files::open_data_file(manifest_file);
    if (contents.has_value()) {
        contents.value() >> manifest;
    } else {
        LOG_WARNING(
            logging::file_io_logger, "Cannot open file {}.", manifest_file.string()
        );
    }

    if (!manifest.isObject()) {
        LOG_CRITICAL(logging::file_io_logger, "No Manifest files");
        return;
    }

    world::entity::ObjectHandler& object_handler =
        world::entity::ObjectHandler::instance();

    for (const Json::Value& entity_data : manifest["entities"]) {
        std::filesystem::path entity_path =
            files::get_data_path() / entity_data["path"].asString();

        // Will check if path exists
        GlobalContext& context = GlobalContext::instance();
        auto future = context.submit(
            [&object_handler](std::filesystem::path path) {
                object_handler.read_object(path);
            },
            entity_path
        );
    }
}

} // namespace util
