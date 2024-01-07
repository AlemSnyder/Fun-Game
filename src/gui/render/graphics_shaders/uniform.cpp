#include "uniform.hpp"

namespace gui {

namespace shader {

void
Uniforms::bind() {
    for (auto uniform : uniforms_) {
        uniform->bind();
    }
}

const std::vector<std::string>
Uniforms::get_names() const {
    std::vector<std::string> out;

    for (auto uniform : uniforms_) {
        out.push_back(uniform->get_name());
    }

    return out;
}

} // namespace shader

} // namespace gui
