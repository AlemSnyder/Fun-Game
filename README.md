# Fun-Game
I am just practicing c++. Feel free to enjoy.

## Setup

### Windows

1. Install MSys2
2. Install packages:
    ```sh
    pacman -Su \
        base-devel \
        mingw-w64-x86_64-toolchain \
        mingw-w64-x86_64-cmake \
        mingw-w64-x86_64-ninja \
        mingw-w64-x86_64-glew \
        mingw-w64-x86_64-glfw \
        mingw-w64-x86_64-glm \
        mingw-w64-x86_64-lua  # not 100% sure this is the right lua version
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

## Formatting

POSIX shells only

```sh
shopt -s extglob                   # For making ** apply to any number of dirs
clang-format src/**/*.{h,c}pp -i
```

Personally I like this oen better:

```sh
git ls-files --exclude-standard | grep -E '\.(cpp|hpp|c|cc|cxx|hxx|ixx)$' | xargs clang-format -i
```
