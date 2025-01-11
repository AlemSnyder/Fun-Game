#pragma once

#include <GL/glew.h>
#include "gui/render/gpu_data/data_types.hpp"

namespace gui {

namespace shader {

class UniformExecuter {
 protected:
    const gpu_data::GPUDataType data_type_;
 public:
    UniformExecuter(gpu_data::GPUDataType data_type) : data_type_(data_type) {}
    virtual ~UniformExecuter() = 0;

    virtual void bind(GLuint uid) const = 0;

    virtual inline gpu_data::GPUDataType get_type() const final {
        return data_type_;
    }
};


} // namespace shaders

} // namespace gui
