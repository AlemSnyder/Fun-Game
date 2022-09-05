# Fun-Game
I am just practicing c++. Feel free to enjoy.

## Setup

### Windows

1. Install MSys2
2. Install packages:
    ```
    pacman -Su \
        base-devel \
        mingw-w64-x86_64-toolchain \
        mingw-w64-x86_64-cmake \
        mingw-w64-x86_64-ninja \
        mingw-w64-x86_64-glew \
        mingw-w64-x86_64-glfw \
        mingw-w64-x86_64-glm
    ```

## Building

```
cd build
cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Debug
ninja
```

## Release

```
cd build
cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Release

ninja
ninja package
ninja package_source
```
