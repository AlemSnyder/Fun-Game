
#pragma once

#include "gui/render/gpu_data/array_buffer.hpp"
#include "gui/render/gpu_data/gpu_data.hpp"
#include "gui/render/gpu_data/static_mesh.hpp"
#include "json/json.h"
#include "placement.hpp"
#include "types.hpp"
#include "util/voxel.hpp"

#include <filesystem>
#include <unordered_set>
#include <vector>

namespace world {

namespace entity {

constexpr size_t NO_UPDATE = -1;

class ModelController;

// Think of this class as rose bush
// there are multiple rose bush models
// all model no positions
// Own file
class ObjectData {
 private:
    // std::vector<voxel_utility::VoxelObject> models_;

    std::vector<ModelController> model_meshes_;

    // some size/off set center. Models must have the same size.

    std::string name_;
    std::string identification_;

 public:
    ObjectData(const Json::Value& object_json, std::filesystem::path object_path);

    [[nodiscard]] inline ModelController&
    get_model(size_t mesh_id) {
        return model_meshes_[mesh_id];
    }

    [[nodiscard]] inline size_t
    num_models() const {
        return model_meshes_.size();
    }

    [[nodiscard]] inline auto
    begin() {
        return model_meshes_.begin();
    }

    [[nodiscard]] inline auto
    end() {
        return model_meshes_.end();
    }

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
class ModelController : virtual public gui::gpu_data::GPUDataElementsInstanced {
 private:
    gui::gpu_data::InstancedIMeshGPU model_mesh_;

    std::vector<gui::gpu_data::Texture1D> model_textures_;

    // each mesh has a different texture
    gui::gpu_data::ArrayBuffer<uint8_t> texture_id_;

    // vector of placements PlacementOrder is the hash function
    std::unordered_set<Placement, PlacementOrder> placements_;

    size_t offset_ = NO_UPDATE;

    // multiple array buffers

    // scale const int how much the model is scaled

    inline void
    reset_offset() {
        offset_ = NO_UPDATE;
    }

 public:
    // some way to generate an instanced Int mesh renderer, except with scale

    void insert(Placement placement);

    void remove(Placement placement);

    void
    add_texture(std::vector<ColorFloat> color_texture_data) {
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this, color_texture_data]() {
            model_textures_.emplace_back(color_texture_data);
        });
    }

    /**
     * @brief Send all placement data to gpu. Should only be run once per frame.
     */
    void update();

    ModelController(const world::entity::Mesh& model_mesh) :
        model_mesh_(model_mesh, {}) {}

    inline void
    bind() const override {
        model_mesh_.bind();
        texture_id_.bind(4);
        // TODO change to 2D texture
        model_textures_[0].bind(0);
    }

    inline void
    release() const override {
        model_mesh_.release();
    }

    [[nodiscard]] inline bool
    do_render() const override {
        return model_mesh_.do_render();
    }

    [[nodiscard]] inline uint32_t
    get_num_vertices() const override {
        return model_mesh_.get_num_vertices();
    }

    [[nodiscard]] inline gui::gpu_data::GPUDataType
    get_element_type() const override {
        return model_mesh_.get_element_type();
    }

    [[nodiscard]] inline uint32_t
    get_num_models() const override {
        return placements_.size();
    }
};

} // namespace entity

} // namespace world
