#include <glm/glm.hpp>

#include <array>
#include <cstdint>

using Dim = uint16_t;

// four chanels of 8 bits
using ColorInt = uint32_t;
// color chanels as vec of ints
using color_t = glm::u8vec4;
// four chanels of floats
using ColorFloat = glm::vec4; // std::array<float, 4>;

using MaterialId = uint8_t;
using ColorId = uint8_t;

// should be size of MaterialId + size of ColorId
using MatColorId = uint16_t;
static_assert(sizeof(MaterialId) + sizeof(ColorId) == sizeof(MatColorId));
