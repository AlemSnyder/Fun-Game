// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/**
 * @file voxel.hpp
 *
 * @brief Defines VoxelBase and VoxelObject classes.
 *
 * @ingroup Util
 *
 */

#pragma once

#include "glm/glm.hpp"
#include "types.hpp"

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace voxel_utility {

class VoxelBase {
 public:
    ColorInt get_voxel(VoxelDim x, VoxelDim y, VoxelDim z) const;
    ColorInt get_voxel(const VoxelOffset& position) const;
    VoxelColorId get_voxel_color_id(VoxelDim x, VoxelDim y, VoxelDim z) const;
    ColorInt get_voxel_color_id(const VoxelOffset& position) const;
    std::vector<ColorInt> get_color_ids() const;
    VoxelSize get_size() const;
    VoxelOffset get_offset() const;
};

template <class T>
concept VoxelLike = std::is_base_of<voxel_utility::VoxelBase, T>::value;

class VoxelObject : VoxelBase {
 private:
    std::vector<VoxelColorId> data_;
    std::vector<ColorInt> colors_;
    VoxelOffset center_;
    VoxelSize size_;
    bool ok_;

    [[nodiscard]] inline size_t //TileIndex
    get_position(VoxelDim x, VoxelDim y, VoxelDim z) const {
        return ((x * size_.y + y) * size_.z + z);
    }

 public:
    /**
     * @brief Construct a new Voxel Object object from saved qb
     *
     * @param path path to .qb file
     */
    VoxelObject(const std::filesystem::path& path);

    /**
     * @brief Construct a new Voxel Object object from saved qb
     *
     * @param path path to .qb file
     */
    VoxelObject(const std::string& path) : VoxelObject(std::filesystem::path(path)) {}

    /**
     * @brief did this voxel object load correctly
     *
     * @return true loaded correctly
     * @return false failed to load correctly
     */
    [[nodiscard]] inline bool
    ok() const noexcept {
        return ok_;
    }

    /**
     * @brief Get the color ids vector
     *
     * @return std::vector<ColorInt>
     */
    [[nodiscard]] inline const std::vector<ColorInt>&
    get_color_ids() const {
        return colors_;
    }

    /**
     * @brief Get the voxel color id
     *
     * @param x x position
     * @param y y position
     * @param z z position
     * @return VoxelColorId
     */
    [[nodiscard]] inline VoxelColorId
    get_voxel_color_id(VoxelDim x, VoxelDim y, VoxelDim z) const {
        if (x < 0 || y < 0 || z < 0) {
            return 0;
        }
        if ((size_.x > static_cast<glm::uint32>(x))
            && (size_.y > static_cast<glm::uint32>(y))
            && (size_.z > static_cast<glm::uint32>(z))) {
            return data_[get_position(x, y, z)];
        }
        return 0;
    }

    /**
     * @brief Get the voxel color id
     *
     * @param VoxelOffset x, y, z position
     * @return VoxelColorId
     */
    [[nodiscard]] inline VoxelColorId
    get_voxel_color_id(const VoxelOffset& position) const {
        return get_voxel_color_id(position.x, position.y, position.z);
    }

    /**
     * @brief Get the voxel color at given coordinate
     *
     * @param x coordinate
     * @param y coordinate
     * @param z coordinate
     * @return ColorInt color
     */
    [[nodiscard]] inline ColorInt
    get_voxel(VoxelDim x, VoxelDim y, VoxelDim z) const {
        return colors_[get_voxel_color_id(x, y, z)];
    }

    /**
     * @brief Get the center of the object use full to find where to rotate
     * around
     *
     * @return VoxelOffset
     */
    [[nodiscard]] inline VoxelOffset
    get_offset() const noexcept {
        return center_;
    }

    /**
     * @brief Get the size as an array of length three
     *
     * @return VoxelSize length in x, y, z
     */
    [[nodiscard]] inline VoxelSize
    get_size() noexcept {
        return size_;
    }
};

struct qb_data_t {
    std::vector<ColorInt> data;
    VoxelOffset center;
    VoxelSize size;
};

} // namespace voxel_utility
