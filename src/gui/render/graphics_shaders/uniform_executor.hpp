#pragma once

#include "gui/render/gpu_data/data_types.hpp"

#include <GL/glew.h>

namespace gui {

namespace shader {

class UniformExecutor {
 protected:
    const gpu_data::GPUDataType data_type_;

 public:
    UniformExecutor(gpu_data::GPUDataType data_type) : data_type_(data_type) {}

    virtual inline ~UniformExecutor() {}

    virtual void bind(GLint uid) const = 0;

    inline gpu_data::GPUDataType
    get_type() const {
        return data_type_;
    }
};

} // namespace shader

} // namespace gui
