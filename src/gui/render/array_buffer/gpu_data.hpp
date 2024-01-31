
#pragma once

#include "array_buffer.hpp"

#include <stdint.h>

namespace gui {

namespace array_buffer {

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

    virtual GPUDataType get_element_type() const = 0;
};

class GPUDataInstanced : virtual public GPUData {
 public:
    virtual uint32_t get_num_models() const = 0;
};

class GPUDataElementsInstanced : virtual public GPUDataElements {
 public:
    virtual uint32_t get_num_models() const = 0;
};

} // namespace array_buffer

} // namespace gui
