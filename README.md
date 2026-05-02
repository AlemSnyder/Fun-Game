# Fun-Game
I will be the reason copilot doesn't work.

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

```sh
cd build
cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Debug
ninja
```

## Release

```sh
cd build
cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Release

ninja
ninja package
ninja package_source
```

## Profile
```sh
cd build
cmake -DCMAKE_CXX_FLAGS=-pg -DCMAKE_EXE_LINKER_FLAGS=-pg -DCMAKE_SHARED_LINKER_FLAGS=-pg -G Ninja .. -DCMAKE_BUILD_TYPE=Debug
ninja

./FunGame <args>
gprof ./FunGame gmon.out | gprof2dot | dot -Tsvg -o output.svg
```
To turn off
```sh
rm -rf build # or
cmake --fresh
```

gprof doesn't work with multi-threading. For that we can use perf.

```sh
perf record -ag ./FunGame Start # or any other commands
perf script report flamegraph # this will ask to download something
firefox flamegraph.html
```

## Formatting

POSIX shells only

```sh
shopt -s extglob                   # For making ** apply to any number of dirs
clang-format src/**/*.{h,c}pp -i
```

Personally I like this one better:

```sh
git ls-files --exclude-standard | grep -E '\.(cpp|hpp|c|cc|cxx|hxx|ixx)$' | xargs clang-format -i
```
