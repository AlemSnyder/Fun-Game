
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
class ModelController : virtual public gui::gpu_data::GPUDataElements {
 private:
    gui::gpu_data::StaticMesh model_mesh_;

    // std::vector<GLuint> model_textures_;

    // each mesh has a different texture
    gui::gpu_data::ArrayBuffer<uint8_t> model_textures_;

    // vector of placements PlacementOrder is the hash function
    std::unordered_set<Placement, PlacementOrder> placements_;

    size_t offset_ = NO_UPDATE;

    // multiple array buffers

    // ObjectData object_data_;

    // array buffer generated from the model
    // and one from the transforms

    // scale const int how much the model is scaled

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

    // TODO
    // this is ill formed as model mesh cannot be copied
    ModelController(world::entity::Mesh model_mesh) : model_mesh_(model_mesh) {}

    void
    bind() const override {
        model_mesh_.bind();
    }

    void
    release() const override {
        model_mesh_.release();
    }

    bool
    do_render() const override {
        return model_mesh_.do_render();
    }

    uint32_t
    get_num_vertices() const override {
        return model_mesh_.get_num_vertices();
    }

    gui::gpu_data::GPUDataType
    get_element_type() const override {
        return model_mesh_.get_element_type();
    }
};

} // namespace entity

} // namespace world
