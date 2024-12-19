# Graphics Pipeline

## Low Level Interfaces


### Vertex Buffer Object

Light C++ wrapper that handles sending data to gpu. 

### Vertex Array Object

Used to automatically bind Vertex Buffer Objects.

### Data Types

Data types describes the four ways data can be read from buffers. There may be
elements and there may be instancing. The four possibilities are given below.

|             | No Instancing   | Instancing               |
|-------------|-----------------|--------------------------|
| No Elements | GPUData         | GPUDataInstanced         |
| Elements    | GPUDataElements | GPUDataElementsInstanced |

## Structures

### Int Mesh

`i_mesh.hpp -> gui::gpu_data::IMeshGPU`

Structure that describes voxel. Contains vertex positions colors ids, normal directions, and elements.

One should add the color texture to render things correctly.

For example `TerrainMesh` and `StaticMesh` both inherit from `IntMesh` and
implement a color texture. (1D and 2D respectively.) 

### Instanced Int Mesh

These are just models. The same mesh in multiple positions.

Again inheritance like above. `Model` inherits from `InstancedIMeshGPU`.

### Floating Instanced I Mesh

Instanced Int Mesh with a float transpose.

### Stars

This also exists

### Scene

Just the four corners of the screen.

## Programs

Bad right now. Everything is handled in scene setup.

## Uniforms

TODO implement uniform buffers.

Uniforms are functionally uniform buffers, just slightly worse.

Also need to remove the cursed regex. Just use the compiled shaders.

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

meshloader.hpp no longer exists
