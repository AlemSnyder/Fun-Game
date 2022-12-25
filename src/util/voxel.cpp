    
    
    
#include "voxel.hpp"
#include "voxel_io.hpp"

namespace voxel_utility {

VoxelObject::VoxelObject(const std::filesystem::path path){
    std::vector<uint32_t> voxel_colors;
    try {
        from_qb(path, voxel_colors, center_, size_);
        ok_ = true;
    } catch (const std::exception& e) {
        LOG_ERROR(
            logging::file_io_logger, "Could not create VoxelObject: {}", e.what()
        );
        ok_ = false;
    }
    // empty should always have index 0;
    colors_.push_back(0);
    for (std::size_t i = 0; i < voxel_colors.size(); i++){
        uint16_t j = 0;
        for (; j < colors_.size(); j++) {
            if (colors_[j] == voxel_colors[i]){
                goto on_break;
            }
        }
        // no break
        // voxel_colors[i] is not in colors
        // we should add it to colors
        // the index is the length of colors because it will be appended
        j = colors_.size();
        // add the color
        colors_.push_back(voxel_colors[i]);
        
        on_break:
        data_.push_back(j);

    }
    if (colors_.size() > 1U << 15){
        LOG_ERROR(
            logging::voxel_logger, "Too many colors in voxel file: {}", colors_.size()
        );
    }
}

} // namespace voxel_utility
