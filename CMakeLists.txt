cmake_minimum_required(VERSION 3.10)

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
)

# Link and/or include our dependencies
target_include_directories(rtiow PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(rtiow PRIVATE WebP::webp)
target_link_libraries(rtiow PRIVATE argparse::argparse)
