#include "terrain_mesh.hpp"

namespace gui {

namespace gpu_data {

namespace detail {

coalesced_data::coalesced_data(const std::unordered_map<ChunkPos, world::entity::Mesh> mesh_map) {
        size_t total_size = 0;
        size_t total_elements_size = 0;
        for (const auto& [pos, mesh] : mesh_map) {
            total_size += mesh.get_indexed_vertices().size();
            total_elements_size += mesh.get_indices().size();
        }
        vertex_array.reserve(total_size);
        color_array.reserve(total_size);
        normal_array.reserve(total_size);

        element_array.reserve(total_elements_size);

        size_t offset_size = 0;
        size_t vertex_offset_size = 0;

        num_vertices.reserve(mesh_map.size());
        elements_offsets.reserve(mesh_map.size());
        base_vertex.reserve(mesh_map.size());

        for (const auto& [pos, mesh] : mesh_map) {
            vertex_array.insert(
                vertex_array.end(), mesh.get_indexed_vertices().begin(),
                mesh.get_indexed_vertices().end()
            );
            color_array.insert(
                color_array.end(), mesh.get_indexed_color_ids().begin(),
                mesh.get_indexed_color_ids().end()
            );
            normal_array.insert(
                normal_array.end(), mesh.get_indexed_normals().begin(),
                mesh.get_indexed_normals().end()
            );

            element_array.insert(
                element_array.end(), mesh.get_indices().begin(),
                mesh.get_indices().end()
            );

            num_vertices.push_back(mesh.get_indices().size());
            elements_offsets.push_back(offset_size * sizeof(decltype(element_array)::value_type));
            base_vertex.push_back(vertex_offset_size);

            offset_size += mesh.get_indices().size();
            vertex_offset_size += mesh.get_indexed_vertices().size();

            if (offset_size != element_array.size()) {
                LOG_WARNING(logging::opengl_logger, "Offset size: {} and element array size {} not equal", offset_size, element_array.size());

            }

            auto max_element = std::max_element(mesh.get_indices().begin(), mesh.get_indices().end());

            if (*max_element != mesh.get_indexed_vertices().size() - 1) {
                LOG_WARNING(logging::opengl_logger, "Max element: {} and indices offset {} not equal", *max_element, mesh.get_indices().size() - 1);

            }
        }
/*
        LOG_INFO(logging::opengl_logger, "elements array size: {} expected size: {}", element_array.size(), total_elements_size);

        LOG_INFO(logging::opengl_logger, "vertex array size: {} expected size: {}", vertex_array.size(), total_size);

        LOG_INFO(logging::opengl_logger, "Elements {}", element_array);

        LOG_INFO(logging::opengl_logger, "Num Vertices {}", num_vertices); // sizes

        LOG_INFO(logging::opengl_logger, "Elements Offsets {}", elements_offsets);

        LOG_INFO(logging::opengl_logger, "Base Vertex {}", base_vertex);
*/

        for (size_t j = 0; j < mesh_map.size(); j++) {
            size_t offset = elements_offsets[j];
            LOG_INFO(logging::opengl_logger, "Index {}.", j);
            LOG_INFO(logging::opengl_logger, "Num Vertices {}, Elements offsets {}, base vertex {}.", num_vertices[j], offset, base_vertex[j]);

            LOG_INFO(logging::opengl_logger, "Elements [{}, {}, {}, {}, {}, {},...", element_array[offset], element_array[offset+1], element_array[offset+2], element_array[offset+3], element_array[offset+4], element_array[offset+5] );
        }

    }
}

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
    // update base_vertex_
    if (base_vertex_.size() > 0) {
        size_t size = base_vertex_.size();
        base_vertex_.push_back( vertex_array_.size() );
    } else {
        base_vertex_.push_back(0);
    }
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

    // update base_vertex_
    if (index == num_vertices_.size()-1) {
        if (index != 0){
            base_vertex_[index] = base_vertex_[index - 1] + mesh.get_indexed_vertices().size();
        } else {
            base_vertex_[index] = mesh.get_indexed_vertices().size();
        }
    } else {
        size_t difference = mesh.get_indexed_vertices().size() - (base_vertex_[index + 1] - base_vertex_[index]);

        for (size_t i = index + 1; i < num_vertices_.size(); i++) {
            base_vertex_[index] += difference;
        }
    }
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

    // update base_vertex_
    if (index != num_vertices_.size()-1) {
        size_t difference = (base_vertex_[index] - base_vertex_[index + 1]);
        for (size_t i = index + 1; i < num_vertices_.size(); i++) {
            base_vertex_[index] += difference;
        }
    }
    base_vertex_.erase(base_vertex_.begin() + index);

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
