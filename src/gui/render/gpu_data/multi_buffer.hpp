#pragma once

// #include "mesh.hpp"
#include "vertex_buffer_object.hpp"

#include <cstdint>
#include <tuple>
#include <unordered_set>
#include <vector>

struct DrawElementsIndirectCommand {
    uint count;         // Specifies the number of elements to be rendered.
    uint instanceCount; // Specifies the number of instances of the indexed geometry
                        // that should be drawn. (1)
    uint
        firstIndex; // Specifies an offset to the location where the indices are stored.
    int baseVertex; // Specifies a constant that should be added to each element of
                    // indices when chosing elements from the enabled vertex arrays.
    uint baseInstance; // Specifies the base instance for use in fetching instanced
                       // vertex attributes. (0)
};

namespace gui {

namespace gpu_data {

template <class... Data_Types>
class MultiBuffer {
    std::vector<DrawElementsIndirectCommand> draw_commands_;
    std::vector<uint> data_array_end_;

    std::vector<size_t> id_to_index_;
    std::unordered_set<size_t> unused_ids_;

    VertexBufferObject<uint16_t, BindingTarget::ELEMENT_ARRAY_BUFFER>
        element_array_buffer_;

    std::tuple<VertexBufferObject<Data_Types>...> data_;

    //    VertexBufferObject<glm::ivec3> vertex_position_buffer_;

 public:
    MultiBuffer();

    //    bool reserve(size_t mesh_number, size_t mesh_size, size_t elements_size);

    bool update_component(
        size_t id, std::vector<uint16_t> elements,
        std::tuple<std::vector<Data_Types>...> data
    );

    bool update_component(
        size_t id, std::vector<uint16_t> elements, std::vector<Data_Types>... data
    );

    size_t
    add_component(std::vector<uint16_t> elements, std::vector<Data_Types>... data);

    void free_component(size_t id);

    void bind() const;

    void release() const;

    std::tuple<std::vector<Data_Types>...> get_all_data() const;

    std::vector<std::uint16_t> get_element_data() const;

 private:
    bool update_by_index_(
        size_t index, const std::vector<uint16_t>& elements,
        std::tuple<std::vector<Data_Types>...> data
    );
};

template <class... Data_Types>
MultiBuffer<Data_Types...>::MultiBuffer() {}

// maybe don't do this
/*template< class... Data_Types>
bool MultiBuffer<Data_Types...>::reserve(size_t mesh_number, size_t mesh_size, size_t
elements_size) {

    std::apply([](auto& array){ array.write(mesh_number * mesh_size); }, data_);

    array.write(elements_size);

}*/

template <class... Data_Types>
bool
MultiBuffer<Data_Types...>::update_by_index_(
    size_t index, const std::vector<uint16_t>& elements,
    std::tuple<std::vector<Data_Types>...> data
) {
    auto& command = draw_commands_[index];

    const size_t vertex_size = std::get<0>(data_).size();

    for (size_t i = 0; i < std::tuple_size(data_); i++) {
        assert(vertex_size == std::get<i>(data_).size());

        const auto& new_array_data = std::get<i>(data);
        std::get<i>(data_).insert(
            new_array_data, command.baseVertex, data_array_end_[i]
        );
    }

    element_array_buffer_.insert(
        elements, command.firstIndex, command.firstIndex + command.count
    );

    const int size_difference = elements.size() - command.count;
    const int vertex_size_difference =
        vertex_size - command.baseVertex + data_array_end_[index];

    command.count = elements.size();

    // update first index of everything past id
    for (size_t further_index = index + 1; further_index < draw_commands_.size();
         further_index++) {
        draw_commands_[further_index].firstIndex += size_difference; // assert on size
        draw_commands_[further_index].baseVertex += vertex_size_difference;
    }

    return true;
}

template <class... Data_Types>
bool
MultiBuffer<Data_Types...>::update_component(
    size_t id, std::vector<uint16_t> elements,
    std::tuple<std::vector<Data_Types>...> data
) {
    return update_by_index_(id_to_index_[id], elements, data);
}

template <class... Data_Types>
bool
MultiBuffer<Data_Types...>::update_component(
    size_t id, std::vector<uint16_t> elements, std::vector<Data_Types>... data
) {
    return update_by_index_(id_to_index_[id], elements, std::make_tuple(data...));
}

template <class... Data_Types>
size_t
MultiBuffer<Data_Types...>::add_component(
    std::vector<uint16_t> elements, std::vector<Data_Types>... data
) {
    size_t out;

    if (!unused_ids_.empty()) {
        auto iterator = unused_ids_.begin();
        out = *iterator;
        unused_ids_.erase(iterator);
    } else {
        out = id_to_index_.size();
        id_to_index_.emplace_back();
    }

    size_t index = draw_commands_.size();

    uint count = 0;         // number of elements: will be assigned later
    uint instanceCount = 1; // 1 (using terrain only one instance)
    uint firstIndex = element_array_buffer_.size(); // begin of indecies array
    int baseVertex = std::get<0>(data_).size();     // begin of data arrays
    uint baseInstance = 0;                          // 0 no instances

    DrawElementsIndirectCommand command(
        count, instanceCount, firstIndex, baseVertex, baseInstance
    );

    draw_commands_.push_back(command);

    update_component(out, elements, data...);

    return out;
}

template <class... Data_Types>
void
MultiBuffer<Data_Types...>::free_component(size_t id) {
    // set data to nothing

    size_t index = id_to_index_[id];

    update_component(index, {}, {}); // write zero in array

    draw_commands_.erase(draw_commands_.begin() + index);
    data_array_end_.erase(data_array_end_.begin() + index);

    for (size_t& greater_index : id_to_index_) {
        if (greater_index > index) {
            greater_index--;
        }
    }
    unused_ids_.insert(id);
}

template <class... Data_Types>
void
MultiBuffer<Data_Types...>::bind() const {}

template <class... Data_Types>
void
MultiBuffer<Data_Types...>::release() const {}

template <class... Data_Types>
std::tuple<std::vector<Data_Types>...>
MultiBuffer<Data_Types...>::get_all_data() const {
    std::tuple<std::vector<Data_Types>...> out;

    for (size_t i = 0; i < std::tuple_size(data_); i++) {
        //        assert(vertex_size == std::get<i>(data_).size());

        std::get<i>(out) = std::get<i>(data_).read();
    }

    return out;
}

template <class... Data_Types>
std::vector<std::uint16_t>
MultiBuffer<Data_Types...>::get_element_data() const {
    return element_array_buffer_.read();
};

} // namespace gpu_data

} // namespace gui
