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

include (FetchContent)

# ImGui
message(STATUS "Fetching ImGui library")
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.92.0
)
FetchContent_MakeAvailable(imgui)
add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw)

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
find_package(OpenGL REQUIRED)
find_package(assimp CONFIG REQUIRED)
if(UNIX)
  find_package(PkgConfig QUIET)

  if (NOT PkgConfig_FOUND)
    message(FATAL_ERROR "PkgConfig not found, please install it.")
  endif()

  pkg_check_modules(WAYLAND REQUIRED wayland-client)

  if(WAYLAND_FOUND)
    message(STATUS "Running under Wayland, checking for system GLFW")

    pkg_check_modules(GLFW3 QUIET glfw3)

    if(NOT GLFW3_FOUND)
      message(FATAL_ERROR "GLFW with Wayland support not found! Please install GLFW with Wayland support.")
    else()
      message(STATUS "System GLFW found")
      include_directories(${GLFW3_INCLUDE_DIRS})
      link_directories(${GLFW3_LIBRARY_DIRS})
    endif()
  else()
    find_package(glfw3 CONFIG REQUIRED)
  endif()

  list(APPEND EXTRA_LIBS m)
elseif(WIN32)
  find_package(glfw3 CONFIG REQUIRED)
endif()

target_link_libraries( PlanetGenerator
  PRIVATE
    glad
    glm::glm
    glfw
    OpenGL::GL
    assimp::assimp
    imgui
)

set(SHADERS_FOLDER "${CMAKE_SOURCE_DIR}/Shaders/graphics")
set(INCLUDE_SHADERS_FOLDER "${CMAKE_SOURCE_DIR}/Shaders/include")
set(IMAGES_FOLDER "${CMAKE_SOURCE_DIR}/Images")
set(MODEL_FOLDER "${CMAKE_SOURCE_DIR}/Models")

add_custom_target(copy_assets ALL
  # Shaders
  COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:PlanetGenerator>/Shaders
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADERS_FOLDER} $<TARGET_FILE_DIR:PlanetGenerator>/Shaders/RenderShaders
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${INCLUDE_SHADERS_FOLDER} $<TARGET_FILE_DIR:PlanetGenerator>/Shaders/Includes
  # Images
  COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:PlanetGenerator>/Images
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${IMAGES_FOLDER} $<TARGET_FILE_DIR:PlanetGenerator>/Images
  # Models
  COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:PlanetGenerator>/Models
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${MODEL_FOLDER} $<TARGET_FILE_DIR:PlanetGenerator>/Models
)
