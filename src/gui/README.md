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

Data structures
    Mesh
        a mesh that is not instanced, and does not change with time
    InstancedMesh
    TransformMesh
    InstancedTransformMesh

    More data structures if I want to do material surfaces
        reflectance glow blume

    In addition there are non-mesh data structures
        screen shape
        any particle affects

Vertex shaders
    each data structure will have its own vertex shader
        by definition.
    Actually changing lighting and glow don't need their own vertex 
        shader. however adding them to the color texture won't be that 
        expensive.
    May data structures can share the same fragment shader.
        Mesh, InstancedMesh, TransformMesh, InstancedTransformMesh will 
        all use the same fragment shader for main render, and shadow map
    
Fragment shaders
maybe this to include things
https://registry.khronos.org/OpenGL/extensions/ARB/ARB_shading_language_include.txt
This may allow compiling from different paths
https://stackoverflow.com/a/25987964
