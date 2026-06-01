#include "entity.hpp"

#include "glm/gtx/transform.hpp"
#include "local_context.hpp"
#include "logging.hpp"
#include "util/files.hpp"

namespace world {

namespace object {

namespace entity {

Entity::Entity(const util::Mesh& mesh) :
    mesh_and_positions_(
        std::make_shared<gui::gpu_data::FloatingInstancedIMeshGPU>(
            mesh, std::vector<glm::mat4>()
        )
    ) {
    LOG_WARNING(
        logging::main_logger,
        "Entity constructor Entity(const Mesh& mesh) is depreciated!"
    );
}

Entity::Entity(
    const object_t& object_data, const manifest::descriptor_t& identification_data
) : name_(object_data.name), identification_(identification_data.identification) {
    const auto& model_data = object_data.models[0];
    // read mesh from path
    std::filesystem::path object_path_copy = identification_data.path;

    voxel_utility::VoxelObject model(
        files::get_data_path() / object_path_copy.remove_filename() / model_data.path
    );

    auto mesh = util::ambient_occlusion_mesher(model);

    mesh_and_positions_ =
        std::make_shared<gui::gpu_data::FloatingInstancedIMeshGPU>(mesh);

    if (object_data.ai) {
        std::filesystem::path ai_path = files::get_data_path()
                                        / object_path_copy.remove_filename()
                                        / object_data.ai.value();

        // load into the angelscript engine
        GlobalContext& context = GlobalContext::instance();
        context.load_file("Base", ai_path);

        has_ai_ = true;
    }
}

std::string
Entity::identification() const {
    return identification_;
}

void
Entity::init_render(render_programs_t& programs) const {
    programs.entity_render_program->data.push_back(mesh_and_positions_.get());
}

// might want to shrink to fit some times
void
Entity::sync_data_to_gpu() {
    mesh_and_positions_->update_transforms_array(local_positions_, 0);
    local_positions_.clear();
}

glm::vec3
Entity::decision(EntityInstance* entity_instance) {
    LocalContext& local_context = LocalContext::instance();

    GlobalContext& global_context = GlobalContext::instance();
    // global_context.get_function(identification_, "vec3 update(Base::entity)");
    // TODO
    // need to add glm vec3 to as
    // and need to add the entity virtual class

    // Then call function and pass the entity instance object to it.

    glm::vec3 position(0);
    return position;
}

void
Entity::add_position(glm::mat4 position) const {
    local_positions_.push_back(position);
}

EntityInstance::EntityInstance(std::shared_ptr<Entity> entity_type) :
    entity_type_(entity_type) {}

EntityInstance::~EntityInstance() {}

void
EntityInstance::operate(std::chrono::milliseconds delta_time, bool show) {
    if (std::shared_ptr<Entity> entity_type = entity_type_.lock()) {
        glm::vec3 position = entity_type->decision(this);
        position_ = position;
        if (show) {
            glm::mat4 transformation(1.0);
            glm::mat4 data = glm::translate(transformation, position_);
            entity_type->add_position(data);
        }
    }
}

glm::vec3
EntityInstance::get_position() const {
    if (std::shared_ptr<Entity> entity_type = entity_type_.lock()) {
        return position_;
    }

    return {0, 0, 0};
}

void
EntityInstance::set_position(glm::vec3 position) {
    if (std::shared_ptr<Entity> entity_type = entity_type_.lock()) {
        position_ = position;
        //        glm::mat4 transformation(1.0);
        //        glm::mat4 data = glm::translate(transformation, position);
        //        entity_type->add_position(data);
    }
}

void
EntityInstance::destroy() {}

std::shared_ptr<Object>
EntityInstance::get_object() {
    return entity_type_.lock();
}

std::shared_ptr<const Object>
EntityInstance::get_object() const {
    return entity_type_.lock();
}

std::shared_ptr<Entity>
EntityInstance::get_entity() {
    return entity_type_.lock();
}

std::shared_ptr<const Entity>
EntityInstance::get_entity() const {
    return entity_type_.lock();
}

} // namespace entity

} // namespace object

} // namespace world
