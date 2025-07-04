#pragma once

#include "util/hash_combine.hpp"

#include <glm/glm.hpp>

#include <array>
#include <cstdint>

/***********
 * Terrain *
 ***********/

// Tile position within a chunk
using LocalPosition = glm::u8vec3;
// Tile position in terrain.
using Dim = uint16_t;
// Tile position in terrain with all dimensions.
using TerrainDim3 = glm::u16vec3;
// the vector indicies must have the same size as dim.
static_assert(sizeof(TerrainDim3) == 3 * sizeof(Dim));
using TerrainOffset = int32_t;
using TerrainOffset3 = glm::i32vec3;
static_assert(sizeof(TerrainOffset3) == 3 * sizeof(TerrainOffset));

using ChunkDim = int16_t;
using ChunkPos = glm::i16vec3;
static_assert(sizeof(ChunkPos) == 3 * sizeof(ChunkDim));

// Macro tile position in macro map.
using MacroDim = uint16_t;

/***************
 * Pathfinding *
 ***************/

// Stores data about path direction and clear-ness
using path_t = uint8_t;

/**********************
 * Terrain Generation *
 *********************/

// Range over all space where a value can be found
using NoisePosition = double;
// Range over all space where random values can be generator for
using NoiseTileIndex = int32_t;

/*************
 * Materials *
 *************/

// four chanels of 8 bits
using ColorInt = uint32_t;
// color chanels as vec of ints
using color_t = glm::u8vec4;
// four chanels of floats
using ColorFloat = glm::vec4; // std::array<float, 4>;

using MaterialId = uint8_t;
using ColorId = uint8_t;

static constexpr MaterialId MAT_ANY_MATERIAL = static_cast<MaterialId>(-1);
static constexpr ColorId COLOR_ANY_COLOR = static_cast<ColorId>(-1);

static constexpr MaterialId AIR_MAT_ID = 0;
static constexpr ColorId AIR_COLOR_ID = 0;

// should be size of MaterialId + size of ColorId
using MatColorId = uint16_t;
static_assert(sizeof(MaterialId) + sizeof(ColorId) == sizeof(MatColorId));

static constexpr MatColorId AIR_MAT_COLOR_ID = 0;

/*********
 * Biome *
 *********/

// possible tile types
using MapTile_t = uint16_t;

using TileMacro_t = uint16_t;

/*************
 * VoxelBase *
 *************/

// color types (ColorInt, color_t, and ColorFloat) are used in VoxelBase, but
// defined above.

using VoxelSize = glm::u32vec3;   // from get_size
using VoxelOffset = glm::i32vec3; // from get_offset

using VoxelDim = int32_t;      // should be signed??
using VoxelColorId = uint16_t; // color id is used to make textures
static_assert(sizeof(VoxelColorId) == sizeof(MatColorId));

// The reason for this static_assert is that VoxelColorId and MatColorId are
// used in templates. We need Terrain base to act like a Voxel base in this
// case.

/**********
 * Opengl *
 **********/

// size in pixels of graphics objects
using screen_size_t = int;

// I'm so sorry, but this is needed because opengl uses ints to return from get
// window size

template <>
struct std::hash<TerrainOffset3> {
    inline size_t
    operator()(const TerrainOffset3& position) const {
        size_t result = 0;
        utils::hash_combine(result, position.x);
        utils::hash_combine(result, position.y);
        utils::hash_combine(result, position.z);
        return result;
    }
};

template <>
struct std::hash<glm::i8vec3> {
    inline size_t
    operator()(const glm::i8vec3& position) const {
        size_t result = 0;
        utils::hash_combine<int8_t>(result, position.x);
        utils::hash_combine<int8_t>(result, position.y);
        utils::hash_combine<int8_t>(result, position.z);
        return result;
    }
};

template <>
struct std::hash<glm::u8vec3> {
    inline size_t
    operator()(const glm::u8vec3& position) const {
        size_t result = 0;
        utils::hash_combine<uint8_t>(result, position.x);
        utils::hash_combine<uint8_t>(result, position.y);
        utils::hash_combine<uint8_t>(result, position.z);
        return result;
    }
};
