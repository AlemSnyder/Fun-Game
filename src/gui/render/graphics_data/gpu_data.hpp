
#pragma once

#include <stdint.h>

namespace gui {

namespace data_structures {

class GPUData {
 public:
    virtual void bind() const = 0;

    virtual void release() const = 0;

    virtual bool do_render() const = 0;

    virtual uint32_t get_num_vertices() const = 0;
};

class GPUDataElements {
 public:
    virtual void bind() const = 0;

    virtual void release() const = 0;

    virtual bool do_render() const = 0;

    virtual uint32_t get_num_vertices() const = 0;

    virtual GLenum get_element_type() const = 0;
};

class GPUDataInstanced : virtual public GPUData {
 public:
    virtual uint32_t get_num_models() const = 0;
};

class GPUDataElementsInstanced : virtual public GPUDataElements {
 public:
    virtual uint32_t get_num_models() const = 0;
};


template <class T>
concept VertexData = std::is_base_of<GPUData, T>::value;

template <class T>
concept VertexElementsData = std::is_base_of<GPUDataElements, T>::value;

template <class T>
concept InstancedVertexData = std::is_base_of<GPUDataInstanced, T>::value;

template <class T>
concept GPUdata_or_something =
    std::is_base_of<GPUData, T>::value || std::is_base_of<GPUDataElements, T>::value;

} // namespace data_structures

} // namespace gui
