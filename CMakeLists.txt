cmake_minimum_required(VERSION 3.10)
#add_compile_options(-municode)


# set the project name
project(Fun_Game)

#target_compile_options(Terrain_Generation -municode)
#add_compile_options(-static-libstdc++)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++")

# add the executable
file(GLOB SOURCES "src/**/*.cpp")
file(GLOB SOURCES2 "src/*.cpp")
add_executable(Fun_Game ${SOURCES} ${SOURCES2})

add_library(ImportVoxel "external/CSDLL.hpp" ${SOURCES} ${SOURCES2})

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

#target_include_directories(Terrain_Generation PRIVATE json)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)