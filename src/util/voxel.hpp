#pragma once

#include "glm/glm.hpp"
#include "../types.hpp"

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace voxel_utility {

class VoxelBase {
 public:
    ColorInt get_voxel(int x, int y, int z) const;
    uint16_t get_voxel_color_id(int x, int y, int z) const;
    std::vector<ColorInt> get_color_ids() const;
    glm::u32vec3 get_size() const;
    glm::i32vec3 get_offset() const;
};

template <class T>
concept VoxelLike = std::is_base_of<voxel_utility::VoxelBase, T>::value;

class VoxelObject : VoxelBase {
 private:
    std::vector<uint16_t> data_;
    std::vector<ColorInt> colors_;
    glm::i32vec3 center_;
    glm::u32vec3 size_;
    bool ok_;

    [[nodiscard]] inline int
    get_position(int x, int y, int z) const {
        return ((x * size_.y + y) * size_.z + z);
    }

 public:
    /**
     * @brief Construct a new Voxel Object object from saved qb
     *
     * @param path path to .qb file
     */
    VoxelObject(const std::filesystem::path path);

    /**
     * @brief Construct a new Voxel Object object from saved qb
     *
     * @param path path to .qb file
     */
    VoxelObject(const std::string path) : VoxelObject(std::filesystem::path(path)) {}

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
     * @return uint16_t
     */
    [[nodiscard]] inline uint16_t
    get_voxel_color_id(int32_t x, int32_t y, int32_t z) const {
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
     * @brief Get the voxel color at given coordinate
     *
     * @param x coordinate
     * @param y coordinate
     * @param z coordinate
     * @return ColorInt color
     */
    [[nodiscard]] inline ColorInt
    get_voxel(int32_t x, int32_t y, int32_t z) const {
        return colors_[get_voxel_color_id(x, y, z)];
    }

    /**
     * @brief Get the center of the object
     * use full to find where to rotate around
     *
     * @return glm::i32vec3
     */
    [[nodiscard]] inline glm::i32vec3
    get_offset() const noexcept {
        return center_;
    }

    /**
     * @brief Get the size as an array of length three
     *
     * @return glm::u32vec3 length in x, y, z
     */
    [[nodiscard]] inline glm::u32vec3
    get_size() noexcept {
        return size_;
    }
};

struct qb_data {
    std::vector<ColorInt> data;
    glm::i32vec3 center;
    glm::u32vec3 size;
};

} // namespace voxel_utility
