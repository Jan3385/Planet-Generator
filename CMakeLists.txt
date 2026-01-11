cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  message(FATAL_ERROR "CMAKE_TOOLCHAIN_FILE is not defined. Make sure you specify it with DCMAKE_TOOLCHAIN_FILE")
endif()

message(STATUS "triplet: ${VCPKG_TARGET_TRIPLET}")
message(STATUS "Using generator: ${CMAKE_GENERATOR}")
message(STATUS "Using VCPKG toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "Preset build type: ${CMAKE_BUILD_TYPE}")

# GLAD
add_library(glad STATIC
    external/glad/src/glad.c
)
target_include_directories(
  glad
  PUBLIC
  external/glad/include
)

project(PlanetGenerator LANGUAGES C CXX)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

add_executable(PlanetGenerator
    ${PROJECT_SOURCES}
)

target_include_directories(PlanetGenerator
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/source
)

# VCPKG packages
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries( PlanetGenerator
  PRIVATE
    glad
    glm::glm
    glfw
    OpenGL::GL
)

set(SHADERS_FOLDER "${CMAKE_SOURCE_DIR}/shader/graphics")
set(INCLUDE_SHADERS_FOLDER "${CMAKE_SOURCE_DIR}/shader/include")

add_custom_target(copy_assets ALL
  # Shaders
  COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:PlanetGenerator>/Shaders
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADERS_FOLDER} $<TARGET_FILE_DIR:PlanetGenerator>/Shaders/RenderShaders
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${INCLUDE_SHADERS_FOLDER} $<TARGET_FILE_DIR:PlanetGenerator>/Shaders/Includes
)
