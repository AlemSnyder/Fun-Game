# Graphics Pipeline

## static voxel

a voxel object that has no time component

### data read from file

### added amalgamated, changed in some way

### voxel like is sent to entity::Mesh of mesh.hpp

### mesh is sent to meshloader.hpp

loaded into graphics memory

### then sent to renderer, and shadow_map

## non-static voxel

### read data from file, and movement file

### any changes

### voxel part is sent ot entity::Mesh of mesh.hpp

### mesh is sent to meshloader.hpp

loaded into graphics memory

### somehow animation data is also sent to meshloader.hpp

### maybe animation data is sent each frame from the cpu to gpu.

## particle affects

