cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# prefer newer OpenGL
# https://cmake.org/cmake/help/latest/policy/CMP0072.html
cmake_policy(SET CMP0072 NEW)

# set the project name
project(Fun_Game)

#target_compile_options(Terrain_Generation -municode)
#add_compile_options(-static-libstdc++)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# add the executable
file(GLOB SOURCES1 "src/*.cpp")
file(GLOB SOURCES2 "src/**/*.cpp")
file(GLOB SOURCES3 "src/**/**/*.cpp")

add_executable(Fun_Game ${SOURCES1} ${SOURCES2} ${SOURCES3})

#target_include_directories(Fun_Game ${GLM_INCLUDE_DIRS})

target_link_libraries(Fun_Game glfw)
target_link_libraries(Fun_Game OpenGL::GL)
target_link_libraries(Fun_Game GLEW::GLEW)
target_link_libraries(Fun_Game glm::glm)

# add_library(ImportVoxel "external/CSDLL.hpp" ${SOURCES} ${SOURCES2})

if(MSVC)
  target_compile_options(Fun_Game PRIVATE /W4 /WX)
else()
  target_compile_options(Fun_Game PRIVATE -Wall -Wextra -Wpedantic)
endif()

#target_link_libraries(jsonparser "src/js")

# src/main.cpp

#set (
#  CMAKE_CXX_FLAGS
#  "${CMAKE_CXX_FLAGS} -municode"
#)

set(CMAKE_BUILD_TYPE Debug)

#target_include_directories(Terrain_Generation PRIVATE json)

