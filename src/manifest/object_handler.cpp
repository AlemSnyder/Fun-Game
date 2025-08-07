#include "object_handler.hpp"

#include "util/files.hpp"
#include "util/voxel.hpp"
#include "world/object/entity/entity.hpp"
#include "world/object/entity/tile_object.hpp"

#include <utility>

namespace manifest {

// TODO change to return a pointer (could be null)
std::shared_ptr<world::object::entity::Object>
ObjectHandler::get_object(const std::string& id) {
    return ided_objects.at(id);
}

std::shared_ptr<const world::object::entity::Object>
ObjectHandler::get_object(const std::string& id) const {
    return ided_objects.at(id);
}

void ObjectHandler::read_biome(const descriptor_t& biome_descriptor) {
    GlobalContext& context = GlobalContext::instance();
    auto biome_data =
        files::read_json_from_file<terrain::generation::biome_data_t>(
            files::get_data_path() / biome_descriptor.path
        );
    if (!biome_data.has_value()) {
        return;
    }

    std::filesystem::path path_copy = biome_descriptor.path;

    context.load_script_file(
        files::get_data_path() / path_copy.remove_filename()
        / biome_data->map_generator_path
    );
}

} // namespace manifest