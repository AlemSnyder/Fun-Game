#pragma once

#include "gui/render/gpu_data/data_types.hpp"

#include <GL/glew.h>

namespace gui {

namespace shader {

class UniformExecuter {
 protected:
    const gpu_data::GPUDataType data_type_;

 public:
    UniformExecuter(gpu_data::GPUDataType data_type) : data_type_(data_type) {}

    virtual inline ~UniformExecuter() {}

    virtual void bind(GLint uid) const = 0;

    virtual inline gpu_data::GPUDataType
    get_type() const final {
        return data_type_;
    }
};

} // namespace shader

} // namespace gui
