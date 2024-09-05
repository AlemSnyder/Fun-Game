#pragma once

#include "gui/placement.hpp"
#include "gui/render/gpu_data/vertex_buffer_object.hpp"
#include "gui/render/structures/model.hpp"
#include "gui/render/structures/static_mesh.hpp"
#include "object.hpp"
#include "types.hpp"
#include "util/voxel.hpp"

#include <mutex>
#include <set>
#include <vector>

namespace world {

namespace entity {

class TileObject;

/**
 * @brief An object placed in the world.
 *
 * @details Anything with a position and a model.
 */
class TileObjectInstance : public virtual ObjectInstance {
 private:
    //    const TileObject& data_;

    // position and rotation
    gui::Placement placement_;

    // object model
    //    gui::render::ModelController& controller_;

    uint8_t model_id_;

    std::shared_ptr<TileObject> object_type_;

 public:
    /**
     * @brief Construct a new TileObject
     *
     * @param ModelController& object model
     * @param gui::Placement position and rotation of object
     */
    TileObjectInstance(
        std::shared_ptr<TileObject> object_type, uint8_t model_id,
        gui::Placement placement
    );
    virtual ~TileObjectInstance();

    /**
     * @brief Equivalents operator
     *
     * @details Used in sets. No two objects should be at the same position.
     */
    [[nodiscard]] inline bool
    operator==(const TileObjectInstance& other) const {
        return placement_ == other.placement_;
    }

    virtual void
    destroy() {}

    virtual size_t
    get_health() const {
        return 1;
    }

    virtual void
    take_damage([[maybe_unused]] size_t damage) {}

    virtual glm::vec3
    get_position() const {
        return {placement_.x, placement_.y, placement_.z};
    }

    virtual std::shared_ptr<Object> get_object();

    virtual std::shared_ptr<const Object> get_object() const;
};

/**
 * @brief Data of Objects of the same type
 *
 * @details Think of this class as rose bush. There are multiple rose bush models, but
 * they have the same size and drops.
 */
class TileObject : public virtual Object {
 private:
    std::vector<gui::render::ModelController> model_meshes_;

    // some size/offset/center. Models must have the same size.

    std::string name_;
    std::string identification_;

 public:
    /**
     * @brief Construct a new TileObject object
     *
     * @details Generates a Object from the json parameters and the path to a voxel
     * object.
     *
     * @param Json::Value& JSON that describes the object
     * @param std::filesystem::path path to folder containing voxel
     TODO fix documentation
     */
    TileObject(const object_t& object_data, const manifest::descriptor_t& model_path);

    /**
     * @brief Get a model for this object by id.
     *
     * @param size_t model mesh id
     *
     * @return ModelController model.
     */
    [[nodiscard]] gui::render::ModelController& get_model(size_t mesh_id);

    [[nodiscard]] const gui::render::ModelController& get_model(size_t mesh_id) const;

    /**
     * @brief Get the number of models that can represent this object.
     *
     * @return size_t the number of models
     */
    [[nodiscard]] size_t num_models() const noexcept;

    /**
     * @brief Iterator to first model
     */
    [[nodiscard]] std::vector<gui::render::ModelController>::iterator begin() noexcept;

    /**
     * @brief Iterator past last element
     */
    [[nodiscard]] std::vector<gui::render::ModelController>::iterator end() noexcept;

    inline TileObject(const TileObject& obj) = delete;
    inline TileObject(TileObject&& other) = default;
    // copy operator
    inline TileObject& operator=(const TileObject& obj) = delete;
    inline TileObject& operator=(TileObject&& other) = default;

    /**
     * @brief Send all ModelController data to gpu. Should be run once per frame
     */
    virtual void update();

    virtual std::string
    identification() const {
        return identification_;
    }

    virtual std::vector<std::shared_ptr<const gui::gpu_data::GPUDataElementsInstanced>>
    renderable_data() const {
        std::vector<std::shared_ptr<const gui::gpu_data::GPUDataElementsInstanced>> out;

        for (const auto& model : model_meshes_) {
            out.emplace_back(&model);
        }

        return out;
    };
};

} // namespace entity

} // namespace world

/*
struct std::hash<world::entity::TileObjectInstance> {
    size_t
    operator()(const world::entity::TileObjectInstance& object) const {
        const auto& pos = object.get_position();
        size_t result = 0;

        // Position
        utils::hash_combine(result, pos.x);
        utils::hash_combine(result, pos.y);
        utils::hash_combine(result, pos.z);

        // The last index is rotation, and this is irreverent to position.
        return result;
    }
};*/
