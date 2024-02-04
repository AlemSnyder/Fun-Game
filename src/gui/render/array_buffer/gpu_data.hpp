
#pragma once

#include "array_buffer.hpp"

#include <stdint.h>

namespace gui {

namespace array_buffer {

/* clang-format off
A graphic of the four GPU Data types. This was along more useful in my mind.

              +-----------------+--------------------------+
              | No Instancing   | Instancing               |
+-------------+-----------------+--------------------------+
| No Elements | GPUData         | GPUDataInstanced         |
+-------------+-----------------+--------------------------+
| Elements    | GPUDataElements | GPUDataElementsInstanced |
+-------------+-----------------+--------------------------+

If something is rendered with elements then rending it without elements will not
work. Like it doesn't even make sense. The order of the vertices will just be
wrong. It would probably be best if the compiler stoped you.

If something that is instanced is rendered without instancing only one item
will be rendered. That's not grate, but its better than with elements.

clang-format on*/

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
