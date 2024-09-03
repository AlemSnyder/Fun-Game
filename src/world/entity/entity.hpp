#pragma once

#include "gui/render/structures/floating_instanced_i_mesh.hpp"
#include "mesh.hpp"

#include <memory>

namespace world {

namespace entity {

class Entity;

class EntityHandler {
    friend Entity;

 private:
    std::shared_ptr<gui::gpu_data::FloatingInstancedIMeshGPU> mesh_and_positions_;

    std::vector<glm::mat4> local_positions_;
    /* data */
 public:
    EntityHandler(const Mesh& mesh);

    void queue_sync();

    void reserve(size_t size);

    [[nodiscard]] inline std::shared_ptr<const gui::gpu_data::FloatingInstancedIMeshGPU>
    data() const {
        return mesh_and_positions_;
    }

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

class Entity {
 private:
    EntityHandler& entity_type_;

    const size_t data_position_; // like a id
 public:
    Entity(EntityHandler& entity_type);

    ~Entity();

    void update();

    void update(glm::mat4&& data);
};

} // namespace entity

} // namespace world
