#include "terrain_mesh.hpp"

namespace gui {

namespace gpu_data {

void
IMeshMultiGPU::attach_all() {
    vertex_array_.attach_to_vertex_attribute(0);
    color_array_.attach_to_vertex_attribute(1);
    normal_array_.attach_to_vertex_attribute(2);
    element_array_.bind();
}

void
IMeshMultiGPU::initialize() {
    vertex_array_object_.bind();
    attach_all();
    vertex_array_object_.release();
}

size_t
IMeshMultiGPU::push_back(const world::entity::Mesh& mesh) {
    vertex_array_.update(mesh.get_indexed_vertices(), vertex_array_.size());
    color_array_.update(mesh.get_indexed_color_ids(), color_array_.size());
    normal_array_.update(mesh.get_indexed_normals(), normal_array_.size());

    num_vertices_.push_back(mesh.get_indexed_vertices().size());
    elements_offsets_.push_back(element_array_.size());

    element_array_.update(mesh.get_indices(), element_array_.size());

    return get_num_objects();
}

void
IMeshMultiGPU::replace(size_t index, const world::entity::Mesh& mesh) {
    assert(index < num_vertices_.size() && "Something Something this will break");
    size_t start = 0;
    for (size_t id = 0; id < index; id++) {
        start += num_vertices_[id];
    }
    size_t end = start + num_vertices_[index];

    vertex_array_.insert(mesh.get_indexed_vertices(), start, end);
    color_array_.insert(mesh.get_indexed_color_ids(), start, end);
    normal_array_.insert(mesh.get_indexed_normals(), start, end);

    start = elements_offsets_[index];
    if (index == num_vertices_.size() - 1) {
        end = element_array_.size();
    } else {
        end = elements_offsets_[index + 1];
    }

    element_array_.insert(mesh.get_indices(), start, end);
}

void
IMeshMultiGPU::remove(size_t index) {
    assert(index < num_vertices_.size() && "Something Something this will break");
    size_t start = 0;
    for (size_t id = 0; id < index; id++) {
        start += num_vertices_[id];
    }
    size_t end = start + num_vertices_[index];

    vertex_array_.insert({}, start, end);
    color_array_.insert({}, start, end);
    normal_array_.insert({}, start, end);

    start = elements_offsets_[index];
    if (index == num_vertices_.size() - 1) {
        end = element_array_.size();
    } else {
        end = elements_offsets_[index + 1];
    }

    element_array_.insert({}, start, end);

    num_vertices_.erase(num_vertices_.begin() + index);
    elements_offsets_.erase(elements_offsets_.begin() + index);
}

void
TerrainMesh::push_back(ChunkPos position, const world::entity::Mesh& mesh) {
    world_position_to_index_[position] = IMeshMultiGPU::push_back(mesh);
}

void
TerrainMesh::replace(ChunkPos position, const world::entity::Mesh& mesh) {
    IMeshMultiGPU::replace(world_position_to_index_[position], mesh);
}

void
TerrainMesh::remove(ChunkPos position) {
    size_t rm_position = world_position_to_index_[position];
    IMeshMultiGPU::remove(rm_position);

    world_position_to_index_.erase(position);

    for (auto& [chunk_position, id] : world_position_to_index_) {
        if (id > rm_position) {
            id--;
        }
    }
}

} // namespace gpu_data

} // namespace gui
