#include <glm/glm.hpp>

#include <array>
#include <cstdint>

using TileIndex = size_t;

using Dim = uint16_t;

using TerrainDim3 = glm::u16vec3;
// the vector indicies must have the same size as dim.
static_assert(sizeof(TerrainDim3) == 3 * sizeof(Dim));
using TerrainOffset = glm::i32vec3;

// For materials
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

// VoxelBase
// color types (ColorInt, color_t, and ColorFloat) are used in VoxelBase, but
// defined above.

using VoxelSize = glm::u32vec3;   // from get_size
using VoxelOffset = glm::i32vec3; // from get_offset

using VoxelDim = int32_t;      // should be signed??
using VoxelColorId = uint16_t; // color id is used to make textures
static_assert(sizeof(VoxelColorId) == sizeof(MatColorId));
