#include "uniform.hpp"

#include <set>

namespace gui {

namespace shader {

std::set<std::pair<std::string, std::string>>
UniformsVector::get_names() const {
    std::set<std::pair<std::string, std::string>> out;

    for (auto uniform : uniforms_) {
        out.emplace(uniform->get_name(), uniform->get_type());
    }

    return out;
}

} // namespace shader

} // namespace gui
