#pragma once

#include "gui/render/structures/floating_instanced_i_mesh.hpp"
#include "manifest.hpp"
#include "mesh.hpp"
#include "object.hpp"

#include <memory>

namespace world {

namespace entity {

class Entity;

class EntityInstance : public virtual ObjectInstance {
 private:
    std::weak_ptr<Entity> entity_type_;

    size_t data_position_; // like a id
 public:
    EntityInstance(std::shared_ptr<Entity> entity_type);

    ~EntityInstance();

    void update();

    void update(glm::mat4&& data);

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
};

class Entity : public virtual Object {
    friend EntityInstance;

 private:
    // data used to render all entities of the same type
    // there will need to be more than just this
    std::shared_ptr<gui::gpu_data::FloatingInstancedIMeshGPU> mesh_and_positions_;

    std::string name_;
    std::string identification_;

    std::vector<glm::mat4> local_positions_;

    bool has_id_;

 public:
    Entity(const Mesh& mesh);

    Entity(const object_t& object_data, const manifest::descriptor_t& model_path);

    virtual ~Entity(){};

    inline void
    reserve(size_t size) {
        local_positions_.resize(size);
    }

    [[nodiscard]] inline std::shared_ptr<const gui::gpu_data::FloatingInstancedIMeshGPU>
    data() const {
        return mesh_and_positions_;
    }

    virtual std::string identification() const;

    virtual void init_render(render_programs_t& programs) const override;

    virtual void update() override;

    [[nodiscard]] inline virtual const std::string&
    get_name() const {
        return name_;
    }

    [[nodiscard]] inline virtual size_t
    num_models() const {
        return 1;
    }

    [[nodiscard]] inline virtual bool
    has_ai() const {
        return has_ai_;
    }

 private:
    inline size_t
    add() {
        insert(glm::mat4());
        return local_positions_.size() - 1;
    }

    inline void
    insert(glm::mat4&& data) {
        local_positions_.push_back(data);
    }

    bool remove(size_t index);

    bool assign(size_t index, glm::mat4& data);
};

} // namespace entity

} // namespace world
