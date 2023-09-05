#pragma once

#include <glm/glm.hpp>

#include <array>
#include <cstdint>

/***********
 * Terrain *
 ***********/

// Tile index in terrain's vector. Also know as pos
using TileIndex = size_t;
using ChunkIndex = size_t;
// Tile position in terrain.
using Dim = uint16_t;
// Tile position in terrain with all dimensions.
using TerrainDim3 = glm::u16vec3;
// the vector indicies must have the same size as dim.
static_assert(sizeof(TerrainDim3) == 3 * sizeof(Dim));
using TerrainOffset = int32_t;
using TerrainOffset3 = glm::i32vec3;
static_assert(sizeof(TerrainOffset3) == 3 * sizeof(TerrainOffset));
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

// should be size of MaterialId + size of ColorId
using MatColorId = uint16_t;
static_assert(sizeof(MaterialId) + sizeof(ColorId) == sizeof(MatColorId));

/*********
 * Biome *
**********/

// possible tile types
using MapTile_t = uint16_t;

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
using screen_size_t = size_t;
