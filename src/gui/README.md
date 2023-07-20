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



game objects -> data structures -> renderers?? -> frame buffers -> scene

each game object has one data structure

why not have one render for each data structure
then pass in the name of the program whe in renders?

that's too much data all of one type should be render by one render at the same time

then four types:
    instanced/individual compressed
    instanced/individual float

then add others:
    some particles will not be instanced floats
    these will be gui particles

TODO remove logic from renderer, and into each data_structures
one virtuial class that handels all renders

The virtuial class will be frame buffer, and shadow map
screen shaped frame buffer, blume, etc.

