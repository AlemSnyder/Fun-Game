#include <array>
#include <cstdint>

#define DEBUG_MATERIAL 7
#define NUM_DEBUG_COLORS 4

using Dim_t = uint16_t;

// four chanels of 8 bits
using Color_int_t = uint32_t;
// four chanels of floats
using Color_float_t = std::array<float, 4>;

using Material_id_t = uint8_t;
using Color_id_t = uint8_t;

// should be size of Material_id_t + size of Color_id_t
using MatColor_id_t = uint16_t;
static_assert(sizeof(uint8_t) + sizeof(uint8_t) == sizeof(uint16_t));
