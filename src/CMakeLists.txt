## set target project
file(GLOB target_src "*.h" "*.cpp" "*/*.cpp" "*/*.h") # look for source files
file(GLOB target_shaders "shaders/*.vert" "shaders/*.frag") # look for shaders
add_executable(terrain_lod ${target_src} ${target_shaders})

set(libraries glad glfw)

## set link libraries
target_link_libraries(terrain_lod glad glfw imgui)

## add local source directory to include paths
target_include_directories(terrain_lod PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

## copy shaders folder to build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

## copy again at the time the current target gets compiled
add_custom_command(
        TARGET terrain_lod POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders
        COMMENT "Copying shaders" VERBATIM
)
