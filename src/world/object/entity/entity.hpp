#pragma once

#include "cognition.hpp"
#include "gui/render/structures/floating_instanced_i_mesh.hpp"
#include "manifest/manifest.hpp"
#include "object.hpp"
#include "types.hpp"
#include "util/mesh.hpp"

#include <chrono>
#include <memory>

namespace world {

namespace object {

namespace entity {

class Entity;

class EntityInstance : public virtual ObjectInstance {
 private:
    std::weak_ptr<Entity> entity_type_;

    glm::vec3 position_;

    void set_position(glm::vec3 position);

 public:
    EntityInstance(std::shared_ptr<Entity> entity_type);

    EntityInstance(std::shared_ptr<Entity> entity_type, glm::vec3 position) :
        EntityInstance(entity_type) {
        set_position(position);
    }

    ~EntityInstance();

    void operate(std::chrono::milliseconds delta_time, bool show);

    //    void update(glm::mat4&& data);

    virtual void destroy();

    inline virtual size_t
    get_health() const {
        return 1;
    }

    inline virtual void
    take_damage([[maybe_unused]] size_t damage) {}

    virtual glm::vec3 get_position() const;

    virtual std::shared_ptr<Object> get_object();

    virtual std::shared_ptr<const Object> get_object() const;

    virtual std::shared_ptr<Entity> get_entity();

    virtual std::shared_ptr<const Entity> get_entity() const;
};

class Entity : public virtual Object, public virtual Cognition {
    friend EntityInstance;

 private:
    // data used to render all entities of the same type
    // there will need to be more than just this
    std::shared_ptr<gui::gpu_data::FloatingInstancedIMeshGPU> mesh_and_positions_;

    std::string name_;
    std::string identification_;

    mutable std::vector<glm::mat4> local_positions_;

    bool has_ai_;

 public:
    Entity(const util::Mesh& mesh);

    Entity(const object_t& object_data, const manifest::descriptor_t& model_path);

    virtual ~Entity(){};

    inline void
    reserve(size_t size) const {
        local_positions_.resize(size);
    }

    [[nodiscard]] inline std::shared_ptr<const gui::gpu_data::FloatingInstancedIMeshGPU>
    data() const {
        return mesh_and_positions_;
    }

    [[nodiscard]] virtual std::string identification() const override;

    virtual void init_render(render_programs_t& programs) const override;

    virtual void sync_data_to_gpu() override;

    [[nodiscard]] inline virtual const std::string&
    get_name() const {
        return name_;
    }

    [[nodiscard]] virtual glm::vec3 decision(EntityInstance* entity_instance) override;

    inline virtual void
    execute_plan([[maybe_unused]] EntityInstance* entity_instance) override {}

    inline virtual void
    make_plan([[maybe_unused]] EntityInstance* entity_instance) override {}

    [[nodiscard]] inline virtual size_t
    num_models() const {
        return 1;
    }

    [[nodiscard]] inline size_t
    num_objects() const {
        return local_positions_.size();
    }

    [[nodiscard]] inline virtual bool
    has_ai() const {
        return has_ai_;
    }

    void add_position(glm::mat4 position) const;
};

} // namespace entity

} // namespace object

} // namespace world
