
#pragma once

#include "gui/render/gpu_data/array_buffer.hpp"
#include "gui/render/gpu_data/gpu_data.hpp"
#include "gui/render/gpu_data/static_mesh.hpp"
#include "manifest.hpp"
#include "placement.hpp"
#include "types.hpp"
#include "util/voxel.hpp"

#include <filesystem>
#include <optional>
#include <unordered_set>
#include <vector>

namespace world {

namespace entity {

struct global_illumination_t {
    float ambient;
    float specular;
    float diffuse;
};

struct remapping_t {
    std::unordered_map<ColorInt, ColorInt> map;

    void read_map(std::unordered_map<std::string, std::string> input);

    std::unordered_map<std::string, std::string> write_map() const;
};

struct model_t {
    std::filesystem::path path;
    std::optional<std::vector<remapping_t>> colors;
    global_illumination_t globals;
};

struct object_t {
    std::string name;
    std::vector<model_t> models;
    // define interactions
    // like on drop etc
    // maybe it has a health
};

constexpr size_t NO_UPDATE = -1;

class ModelController;

/**
 * @brief Data of Objects of the same type
 *
 * @details Think of this class as rose bush. There are multiple rose bush models, but
 * they have the same size and drops.
 */
class ObjectData {
 private:
    std::vector<ModelController> model_meshes_;

    // some size/offset/center. Models must have the same size.

    std::string name_;
    std::string identification_;

 public:
    /**
     * @brief Construct a new ObjectData object
     *
     * @details Generates a Object from the json parameters and the path to a voxel
     * object.
     *
     * @param const object_t& object_data describes the object
     * @param const manifest::descriptor_t& model_path path to folder containing voxel
     */
    ObjectData(const object_t& object_data, const manifest::descriptor_t& model_path);

    /**
     * @brief Get a model for this object by id.
     *
     * @param size_t model mesh id
     *
     * @return ModelController model.
     */
    [[nodiscard]] ModelController& get_model(size_t mesh_id);

    /**
     * @brief Get the number of models that can represent this object.
     *
     * @return size_t the number of models
     */
    [[nodiscard]] size_t num_models() const noexcept;

    /**
     * @brief Iterator to first model
     */
    [[nodiscard]] std::vector<world::entity::ModelController>::iterator
    begin() noexcept;

    /**
     * @brief Iterator past last element
     */
    [[nodiscard]] std::vector<world::entity::ModelController>::iterator end() noexcept;

    inline ObjectData(const ObjectData& obj) = delete;
    inline ObjectData(ObjectData&& other) = default;
    // copy operator
    inline ObjectData& operator=(const ObjectData& obj) = delete;
    inline ObjectData& operator=(ObjectData&& other) = default;

    /**
     * @brief Send all ModelController data to gpu. Should be run once per frame
     */
    void update();
};

// TODO move this to GPU
// mostly position
// there is a model controller for each voxel object model
/**
 * @brief Wrapper of data used by each model.
 *
 * @details Contains the mesh and color map for each model used. It also contains
 * at set of the positions of each model.
 */
class ModelController : virtual public gui::gpu_data::GPUDataElementsInstanced {
 private:
    // model mesh on gpu
    gui::gpu_data::InstancedIMeshGPU model_mesh_;

    // texture map
    gui::gpu_data::Texture2D model_textures_;

    // each mesh has a different texture
    gui::gpu_data::ArrayBuffer<uint8_t> texture_id_;

    // vector of placements PlacementOrder is the hash function
    std::unordered_set<Placement, PlacementOrder> placements_;

    // position of data to be written in next frame
    size_t offset_ = NO_UPDATE;

    // multiple array buffers

    // scale const int how much the model is scaled

    /**
     * @brief Reset the offset for the next frame
     */
    inline void
    reset_offset() {
        offset_ = NO_UPDATE;
    }

 public:
    // some way to generate an instanced Int mesh renderer, except with scale

    void insert(Placement placement);

    void remove(Placement placement);

    /**
     * @brief Send all placement data to gpu. Should only be run once per frame.
     */
    void update();

    /**
     * @brief Construct new ModelController
     *
     * @param const world::entity::Mesh& Mesh data
     * @param const std::vector<std::vector<ColorFloat>>& color map data
     */
    ModelController(
        const world::entity::Mesh& model_mesh,
        const std::vector<std::vector<ColorFloat>>& vector_data
    ) :
        model_mesh_(model_mesh, {}),
        model_textures_(vector_data) {}

    /**
     * @brief Bind data for rendering
     */
    inline void
    bind() const override {
        model_mesh_.bind();
        texture_id_.bind(4);
        model_textures_.bind(0);
    }

    /**
     * @brief Unbind data
     */
    inline void
    release() const override {
        model_mesh_.release();
    }

    /**
     * @brief Should this model be rendered.
     *
     * @return True The model should be rendered.
     * @return False The model should not be rendered.
     */
    [[nodiscard]] inline bool
    do_render() const override {
        return model_mesh_.do_render();
    }

    /**
     * @brief Get the number of vertices in the mesh.
     *
     * @return uint32_t the number of vertices
     */
    [[nodiscard]] inline uint32_t
    get_num_vertices() const override {
        return model_mesh_.get_num_vertices();
    }

    /**
     * @brief Get they data buffer type.
     *
     * @return gui::gpu_data::GPUDataType data buffer type
     */
    [[nodiscard]] inline gui::gpu_data::GPUDataType
    get_element_type() const override {
        return model_mesh_.get_element_type();
    }

    /**
     * @brief Return the number of models.
     *
     * @return uint32_t the number of models
     */
    [[nodiscard]] inline uint32_t
    get_num_models() const override {
        return placements_.size();
    }
};

} // namespace entity

} // namespace world

template <>
struct glz::meta<world::entity::remapping_t> {
    using T = world::entity::remapping_t;

    static constexpr auto value = object("map", custom<&T::read_map, &T::write_map>);
};

template <>
inline glz::detail::any_t::operator std::vector<world::entity::remapping_t>() const {
    assert(false && "Not Implemented");
    return {};
}