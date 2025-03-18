cmake_minimum_required(VERSION 3.10)

# export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# set our C++ standard
set(CMAKE_CXX_STANDARD 20)

# set the project name
project(rtiow)

# by default CMake selects "ALL_BUILD" as the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY VS_STARTUP_PROJECT rtiow)

# add in our dependencies
find_package(Stb REQUIRED)
find_package(WebP CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)

# add the executable
add_executable(rtiow
    # Main
    src/main.cpp

    # Dear ImGui
    deps/imgui/imgui.cpp
    deps/imgui/imgui_demo.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_widgets.cpp
    deps/imgui/backends/imgui_impl_sdl2.cpp
    deps/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

# Link and/or include our dependencies
target_include_directories(rtiow PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(rtiow PRIVATE WebP::webp)
target_link_libraries(rtiow PRIVATE argparse::argparse)

# SDL2
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(rtiow
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

# Dear ImGui
target_include_directories(rtiow PRIVATE
    deps/imgui
    deps/imgui/backends
)

# debug
message("DEBUG: CMake Build Type: ${CMAKE_BUILD_TYPE}")
message("DEBUG: STB Include Dir: ${Stb_INCLUDE_DIR}")
message("DEBUG: Compile commands: ${CMAKE_EXPORT_COMPILE_COMMANDS}")
