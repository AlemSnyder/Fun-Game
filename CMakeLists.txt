cmake_minimum_required(VERSION 3.16)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the project name
project(FunGame)

# add the executable
file(GLOB SOURCES1 "src/*.cpp")
file(GLOB SOURCES2 "src/**/*.cpp")
file(GLOB SOURCES3 "src/**/**/*.cpp")

file(GLOB SOURCES4 "vendor/*.cpp")
file(GLOB SOURCES5 "vendor/**/*.cpp")
file(GLOB SOURCES6 "vendor/**/**/*.cpp")

add_executable(FunGame ${SOURCES1} ${SOURCES2} ${SOURCES3} ${SOURCES4} ${SOURCES5} ${SOURCES6})

# prefer newer OpenGL
# https://cmake.org/cmake/help/latest/policy/CMP0072.html
cmake_policy(SET CMP0072 NEW)

# Find GL packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(FunGame glfw)
target_link_libraries(FunGame OpenGL::GL)
target_link_libraries(FunGame GLEW::GLEW)
if (WIN32)
  target_link_libraries(FunGame glm)
else()
  target_link_libraries(FunGame glm::glm)
endif()

# Add vendored packages
include_directories("vendor")

# Set warning options
if(MSVC)
  target_compile_options(FunGame PRIVATE /W4 /WX)
else()
  target_compile_options(FunGame PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy resources and data
add_custom_target(resources
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources
    COMMENT "copying ${CMAKE_SOURCE_DIR}/resources to ${CMAKE_BINARY_DIR}/resources"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_custom_target(data
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/data
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
    COMMENT "copying ${CMAKE_SOURCE_DIR}/data to ${CMAKE_BINARY_DIR}/data"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_dependencies(FunGame resources)
add_dependencies(FunGame data)
