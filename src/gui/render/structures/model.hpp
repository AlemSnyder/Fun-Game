
#pragma once

#include "gui/placement.hpp"
#include "gui/render/gpu_data/data_types.hpp"
#include "gui/render/gpu_data/vertex_buffer_object.hpp"
#include "manifest.hpp"
#include "static_mesh.hpp"
#include "types.hpp"
#include "util/voxel.hpp"

#include <filesystem>
#include <optional>
#include <unordered_set>
#include <vector>

namespace gui {

namespace render {

constexpr size_t NO_UPDATE = -1;

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
    gui::gpu_data::VertexBufferObject<uint8_t> texture_id_;

    // vector of placements PlacementOrder is the hash function
    std::unordered_set<gui::Placement> placements_;

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

    void insert(gui::Placement placement);

    void remove(gui::Placement placement);

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

    ~ModelController() {}

    inline ModelController(const ModelController& obj) = delete;
    inline ModelController(ModelController&& other) = default;
    // copy operator
    inline ModelController& operator=(const ModelController& obj) = delete;
    inline ModelController& operator=(ModelController&& other) = default;

    /**
     * @brief Bind data for rendering
     */
    inline void
    bind() const override {
        model_mesh_.bind();
        texture_id_.attach_to_vertex_attribute(4);
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

} // namespace render

} // namespace gui
