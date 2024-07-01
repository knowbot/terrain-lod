# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(EXTERNAL_LIBRARIES_SOURCE_PATH ${CMAKE_SOURCE_DIR}/third-party)

# PROJECT
project ("terrain_lod")

# LIBRARIES
add_subdirectory(${EXTERNAL_LIBRARIES_SOURCE_PATH}/glfw)
add_subdirectory(${EXTERNAL_LIBRARIES_SOURCE_PATH}/glad)
add_subdirectory(${EXTERNAL_LIBRARIES_SOURCE_PATH}/imgui)
add_subdirectory(${EXTERNAL_LIBRARIES_SOURCE_PATH}/assimp)

include_directories(
        include
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/glad/include
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/glfw//include
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/imgui/include
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/glm
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/stb
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/assimp/include
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/fpng
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/assimp
        )

# Include sub-projects.
add_subdirectory ("src")
