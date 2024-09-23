#pragma once

#include "vertex_buffer_object.hpp"
#include "mesh.hpp"

#include <tuple>
#include <vector>

 struct DrawElementsIndirectCommand {
        uint  count;        // number of elements
        uint  instanceCount; // 1 (using terrain only one instance)
        uint  firstIndex;   // begin of indecies array
        int  baseVertex;    // begin of data arraays
        uint  baseInstance; // 0 no instances
    } ;

namespace gui {

namespace gpu_data {

template< class... Data_Types>
class MultiBuffer {

    std::vector<DrawElementsIndirectCommand>  draw_commands_;

    VertexBufferObject<uint16_t, BindingTarget::ELEMENT_ARRAY_BUFFER> element_array_buffer_;

    std::tuple<VertexBufferObject<...Data_Types>> data_;

//    VertexBufferObject<glm::ivec3> vertex_position_buffer_;

 public:

    MultiBuffer::MultiBuffer() {}

    bool reserve(size_t mesh_number, size_t mesh_size, size_t elements_size);

    bool update_component(size_t id, std::vector<uint16_t> elements, std::tuple<std::vector<...Data_Types>> data);

    bool update_component(size_t id, std::vector<uint16_t> elements, std::vector<...Data_Types>... data);

    size_t add_component(std::vector<uint16_t> elements, std::vector<...Data_Types>... data);

    void free_component(size_t id);

    void bind() const;

    void release() const;

};

}

}

