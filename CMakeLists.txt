cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# prefer newer OpenGL
# https://cmake.org/cmake/help/latest/policy/CMP0072.html
cmake_policy(SET CMP0072 NEW)

# set the project name
project(Fun_Game)

# Find GL ../packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# add the executable
file(GLOB SOURCES1 "src/*.cpp")
file(GLOB SOURCES2 "src/**/*.cpp")
file(GLOB SOURCES3 "src/**/**/*.cpp")

file(GLOB SOURCES4 "packages/*.cpp")
file(GLOB SOURCES5 "packages/**/*.cpp")
file(GLOB SOURCES6 "packages/**/**/*.cpp")

add_executable(Fun_Game ${SOURCES1} ${SOURCES2} ${SOURCES3} ${SOURCES4} ${SOURCES5} ${SOURCES6})

#target_include_directories(Fun_Game ${GLM_INCLUDE_DIRS})

target_link_libraries(Fun_Game glfw)
target_link_libraries(Fun_Game OpenGL::GL)
target_link_libraries(Fun_Game GLEW::GLEW)
if (WIN32)
  target_link_libraries(Fun_Game glm)
else()
  target_link_libraries(Fun_Game glm::glm)
endif()

# Set warning options
if(MSVC)
  target_compile_options(Fun_Game PRIVATE /W4 /WX)
else()
  target_compile_options(Fun_Game PRIVATE -Wall -Wextra -Wpedantic)
endif()
