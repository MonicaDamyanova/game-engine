cmake_minimum_required(VERSION 3.10)

project(GameEngine VERSION 1.0)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set (CMAKE_CXX_FLAGS "-Wall -Winvalid-pch -Wnon-virtual-dtor -Wextra -Werror -Wno-unused-parameter")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/*.c)

file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_SOURCE_DIR}/src/*.h)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

list(APPEND CMAKE_MODULE_PATH
    "${PROJECT_SOURCE_DIR}/cmake/modules/"
)

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# GLFW
find_package(GLFW REQUIRED)
message(STATUS "Found GLFW in ${GLFW_INCLUDE_DIR}")
message(STATUS "GLFW library is ${GLFW_LIBRARY}")

# GLAD
add_library(GLAD "external/glad/glad.c")

# CGLM 
add_subdirectory(external/cglm/ EXCLUDE_FROM_ALL)
# TODO: Just install it from the AUR and remove as submodule.

set(LIBS ${GLFW_LIBRARY} ${OPENGL_LIBRARY} GLAD cglm_headers)
set(DIRS ${OPENGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIR})

include_directories(
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/src/engine/core"
	"${CMAKE_SOURCE_DIR}/src/engine/core/filesystem"
	"${CMAKE_SOURCE_DIR}/src/engine/core/input"
	"${CMAKE_SOURCE_DIR}/src/engine/core/logger"
	"${CMAKE_SOURCE_DIR}/src/engine/core/memory"
	"${CMAKE_SOURCE_DIR}/src/engine/core/window"
	"${CMAKE_SOURCE_DIR}/src/engine/core/transform"
	"${CMAKE_SOURCE_DIR}/src/engine/renderer"
	"${CMAKE_SOURCE_DIR}/src/engine/physics"
	"${CMAKE_SOURCE_DIR}/external"
	"${CMAKE_SOURCE_DIR}/external/glad/include"
	"${CMAKE_SOURCE_DIR}/external/glad/include/glad"
	"${CMAKE_SOURCE_DIR}/external/cglm"
	"${CMAKE_SOURCE_DIR}/external/stb"
    "/usr/include"
    "/usr/include/libdrm" # May remove
    ${LIBDRM_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} ${LIBS} ${LIBDRM_LIBRARIES} m)
target_include_directories(${PROJECT_NAME} PUBLIC ${DIRS})
