#include "voxel.hpp"

#include "voxel_io.hpp"

namespace voxel_utility {

VoxelObject::VoxelObject(const std::filesystem::path& path) {
    std::vector<ColorInt> voxel_colors;
    try {
        qb_data_t qb_data = from_qb(path);
        center_ = qb_data.layer_data[0].center;
        size_ = qb_data.layer_data[0].size;
        voxel_colors = qb_data.layer_data[0].data;
        ok_ = true;
    } catch (const std::exception& e) {
        LOG_ERROR(
            logging::file_io_logger, "Could not create VoxelObject: {}", e.what()
        );
        ok_ = false;
    }

    // empty should always have index 0;
    colors_.push_back(0);

    for (std::size_t i = 0; i < voxel_colors.size(); i++) {
        const auto& color = voxel_colors[i];

        auto loc = std::find(colors_.begin(), colors_.end(), color);
        auto idx = std::distance(colors_.begin(), loc);

        if (loc == colors_.end()) {
            // not found
            // voxel_colors[i] is not in colors
            // add the color
            colors_.push_back(color);
        }

        // This works as begin() - end() == size()
        data_.push_back(idx);
    }

    if (colors_.size() > 1U << 15) {
        LOG_ERROR(
            logging::voxel_logger, "Too many colors in voxel file: {}", colors_.size()
        );
    }
}

VoxelObject::VoxelObject(qb_layer_data_t layer_data) :
    colors_(layer_data.data), center_(layer_data.center), size_(layer_data.size),
    ok_(true) {
    for (std::size_t i = 0; i < voxel_colors.size(); i++) {
        const auto& color = voxel_colors[i];

        auto loc = std::find(colors_.begin(), colors_.end(), color);
        auto idx = std::distance(colors_.begin(), loc);

        if (loc == colors_.end()) {
            // not found
            // voxel_colors[i] is not in colors
            // add the color
            colors_.push_back(color);
        }

        // This works as begin() - end() == size()
        data_.push_back(idx);
    }
}

LayeredVoxelObject::LayeredVoxelObject(qb_data_t layers) {
    for (qb_layer_data_t layer : layers.layer_data) {
        layers_.push_back(std::make_pair(layer.layer_name, VoxelObject(layer)));
    }
}

} // namespace voxel_utility
