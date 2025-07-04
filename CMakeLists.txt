cmake_minimum_required(VERSION 3.16)

include(cmake/functions.cmake)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set the project name
project(FunGame VERSION 0.1.0)

# add the executable
file(GLOB_RECURSE SOURCES1 "src/*.cpp")

# this is one json file and whereami
file(GLOB SOURCES4 "vendor/whereami/src/*.c*")
file(GLOB SOURCES6 "vendor/imgui/imgui/*.c*")
file(GLOB SOURCES7 "vendor/imgui/imgui/backends/imgui_impl_opengl3.cpp")
file(GLOB SOURCES8 "vendor/imgui/imgui/backends/imgui_impl_glfw.cpp")
add_executable(FunGame ${SOURCES1} ${SOURCES4} ${SOURCES6} ${SOURCES7} ${SOURCES8})

# Set standard version
set_property(TARGET FunGame PROPERTY CXX_STANDARD 23)
set_property(TARGET FunGame PROPERTY C_STANDARD 17)

target_compile_definitions(FunGame PUBLIC GLM_ENABLE_EXPERIMENTAL)

# compile quill and sol2
block()
    set(QUILL_BUILD_EXAMPLES   OFF)
    set(QUILL_BUILD_TESTS      OFF)
    set(QUILL_BUILD_BENCHMARKS OFF)
    set(QUILL_DOCS_GEN         OFF)

    add_subdirectory("vendor/quill")
endblock()

add_subdirectory("vendor/sol2")

add_subdirectory("vendor/glaze")

add_subdirectory("vendor/argh")

# prefer newer OpenGL
# https://cmake.org/cmake/help/latest/policy/CMP0072.html
cmake_policy(SET CMP0072 NEW)

# Link against libdl for dlclose (used in latest IMGUI)
target_link_libraries(FunGame PRIVATE ${CMAKE_DL_LIBS})

# Find GL packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(PNG REQUIRED)
# find_package(OpenMP REQUIRED)

target_link_libraries(FunGame PRIVATE glfw)
target_link_libraries(FunGame PRIVATE OpenGL::GL)
target_link_libraries(FunGame PRIVATE GLEW::GLEW)
target_link_libraries(FunGame PRIVATE PNG::PNG)
# target_link_libraries(FunGame PRIVATE OpenMP::OpenMP_CXX)

if(WIN32)
    target_link_libraries(FunGame PRIVATE glm)
else()
    target_link_libraries(FunGame PRIVATE glm::glm)
endif()

# Lua 5.2 just uses the name "Lua" while 5.1, and 5.0 used "Lua51", and "Lua50"
# respectivly.
find_package(Lua REQUIRED)

include_directories(${LUA_INCLUDE_DIR})

# add quill and sol2
target_link_libraries_system(FunGame PRIVATE quill::quill)

target_link_libraries_system(FunGame PRIVATE sol2::sol2)
target_link_libraries(FunGame PRIVATE ${LUA_LIBRARIES})

# Add include dirs
target_include_directories(FunGame PRIVATE "vendor/whereami/src")
target_include_directories(FunGame PRIVATE "vendor/argh")
target_include_directories(FunGame PRIVATE "vendor/thread-pool/include")
target_include_directories(FunGame PRIVATE "vendor/glaze/include")
target_include_directories(FunGame PRIVATE "vendor/imgui")
target_include_directories(FunGame PRIVATE "vendor/imgui/imgui")
target_include_directories(FunGame PRIVATE "src")
target_include_directories(FunGame PRIVATE "${PROJECT_BINARY_DIR}")

# this is for fmt included in quill
target_include_directories(FunGame PRIVATE "vendor/quill/quill/include/quill/bundled")

# Set warning options
if(MSVC)
  target_compile_options(FunGame PRIVATE /W4 /WX -wd4068)
else()
  target_compile_options(FunGame PRIVATE -Wall -Wextra -Wpedantic -Wno-unknown-pragmas -pedantic -fdiagnostics-color=always)
endif()

# Resource and data handling
if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(DEBUG 1)
  message(" Debug build, data will be loaded from repo root ")
else()
  set(DEBUG 0)
  message(" Debug build, data will be loaded from subdirs ")

  target_compile_definitions(FunGame PUBLIC TILE_SET_TEST)

  add_custom_target(resources
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources
    COMMENT " copying ${CMAKE_SOURCE_DIR}/resources to ${CMAKE_BINARY_DIR}/resources "
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  add_custom_target(data
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/data
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
    COMMENT " copying ${CMAKE_SOURCE_DIR}/data to ${CMAKE_BINARY_DIR}/data "
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )

  add_dependencies(FunGame resources)
  add_dependencies(FunGame data)
endif()

configure_file(src/config.h.in config.h)

enable_testing()

add_test(NAME NoiseTest COMMAND FunGame Test NoiseTest)
add_test(NAME Logging COMMAND FunGame Test Logging)
add_test(NAME ChunkDataTest COMMAND FunGame Test ChunkDataTest)
add_test(NAME LoadManifest COMMAND FunGame Test LoadManifest)
add_test(NAME PathFinderTest COMMAND FunGame Test PathFinderTest)
add_test(NAME LuaMap COMMAND FunGame Test Lua Map)
add_test(NAME LuaLogging COMMAND FunGame Test Lua Logging)
add_test(NAME LuaLoadTime COMMAND FunGame Test Lua LoadTime)
add_test(NAME LuaLoadScript COMMAND FunGame Test Lua LoadScript)
add_test(NAME LuaTransferScript COMMAND FunGame Test Lua TransferScript)

