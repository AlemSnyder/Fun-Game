#include <fstream>
#include <iostream>

#include "voxelutility.hpp"

int VoxelUtility::from_qb(const char * path,
            std::vector<uint32_t>& data,
            std::vector<int>& center,
            std::vector<int>& size){
    //Read the tiles from the path specified, and save

    //This is partially from goxel with GPL license
    std::cout << "Reading form " << path << "\n";
    FILE *file;
    //int x, y, z; // position in terrain
    uint32_t void_, compression; // void int 32 used to read 32 bites without saving it.
    int32_t x_center, y_center, z_center;
    uint8_t v[4];

    file = fopen(path, "rb");
    READ<uint32_t>(void_, file); // version
    //std::cout << void_ << std::endl;
    READ<uint32_t>(void_, file);   // color format RGBA
    //std::cout << void_ << std::endl;(void) void_;
    READ<uint32_t>(void_, file);   // orientation right handed // c
    //std::cout << void_ << std::endl;(void) void_;
    READ<uint32_t>(compression, file);   // no compression
    //std::cout << void_ << std::endl;(void) void_;
    READ<uint32_t>(void_, file);   // vmask
    //std::cout << void_ << std::endl;(void) void_;
    READ<uint32_t>(void_, file);
    //std::cout << void_ << std::endl;(void) void_;
    // none of these are used

    int8_t name_len;
    READ<int8_t>(name_len, file);
    std::cout << "name length: " << (int) name_len << std::endl;
    char* name = (char*) malloc (name_len);
    fread(name, sizeof(char), name_len, file);
    std::string string_name(name);
    std::cout << "Name: " << string_name << std::endl;
    uint32_t X_max, Y_max, Z_max;
    READ<uint32_t>(X_max, file);  // x
    READ<uint32_t>(Z_max, file);  // z
    READ<uint32_t>(Y_max, file);  // y
    size = {(int) X_max, (int) Y_max, (int) Z_max};
    READ<int32_t>(x_center, file); // x
    READ<int32_t>(z_center, file); // z
    READ<int32_t>(y_center, file); // y
    center = {x_center, y_center, z_center};
    std::cout << "    max X: " << X_max << std::endl;
    std::cout << "    max Y: " << Y_max << std::endl;
    std::cout << "    max Z: " << Z_max << std::endl;
    data.resize(X_max * Y_max * Z_max);
    int tiles_read=0;
    if (static_cast<bool>(compression)){
        std::cout << "There is compression!" << std::endl;
    } else {
        data.resize(size[0]*size[1]*size[2]);
        for (int x = 0; x < size[0]; x++)
        for (int z = 0; z < size[2]; z++)
        for (int y = size[1] - 1; y >= 0; y--) {
            //int32_t* color = (int32_t*) malloc(32);
            fread(v, sizeof(uint8_t), 4, file);
            //fread(&color, 32, 1, file); // read the color
            data[(x * size[1] + y) * size[2] + z] = compress_color(v);
            tiles_read ++;
        }
    }
    std::cout << "Voxels read: " << tiles_read << std::endl;
    //free(void_);
    //free(compression);
    //free(name_len);
    //free(name);

    fclose(file);
    return 0;
}
