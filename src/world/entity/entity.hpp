#pragma once

#include "gui/render/structures/floating_instanced_i_mesh.hpp"
#include "mesh.hpp"
#include "manifest.hpp"
#include "object.hpp"

#include <memory>

namespace world {

namespace entity {

class Entity;

class EntityInstance : public virtual ObjectInstance{
 private:
    std::weak_ptr<Entity> entity_type_;

    size_t data_position_; // like a id
 public:
    EntityInstance(std::shared_ptr<Entity> entity_type);

    ~EntityInstance();

    void update();

    void update(glm::mat4&& data);

    virtual void destroy();
    virtual size_t get_health() const;
    virtual void take_damage(size_t damage);
    virtual glm::vec3 get_position() const;

    virtual std::shared_ptr<Object> get_object();

    virtual std::shared_ptr<const Object> get_object() const;
};

class Entity : public virtual  Object{
    friend EntityInstance;

 private:
    // data used to render all entities of the same type
    // there will need to be more than just this
    std::shared_ptr<gui::gpu_data::FloatingInstancedIMeshGPU> mesh_and_positions_;

    std::string name_;
    std::string identification_;

    std::vector<glm::mat4> local_positions_;
 public:
    Entity(const Mesh& mesh);

    Entity(const object_t& object_data, const manifest::descriptor_t& model_path);

    virtual ~Entity() {};

    void reserve(size_t size);

    [[nodiscard]] inline std::shared_ptr<const gui::gpu_data::FloatingInstancedIMeshGPU>
    data() const {
        return mesh_and_positions_;
    }

    virtual std::string identification() const;

    virtual std::vector<const gui::gpu_data::GPUDataElementsInstanced*>
    renderable_data() const override;

    virtual void update() override;

 private:
    inline size_t
    add() {
        insert(glm::mat4());
        return local_positions_.size() - 1;
    }

    void insert(glm::mat4&& data);

    bool remove(size_t index);

    bool assign(size_t index, glm::mat4& data);
};

} // namespace entity

} // namespace world
