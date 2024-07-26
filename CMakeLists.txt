cmake_minimum_required(VERSION 3.10)

# set the project name
project(rtiow)

# by default CMake selects "ALL_BUILD" as the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY VS_STARTUP_PROJECT rtiow)

# add in our dependencies
find_package(Stb REQUIRED)

# add the executable
add_executable(rtiow

    # Headers
    Source/color.hpp
    Source/camera.hpp
    Source/hittable.hpp
    Source/hittable_list.hpp
    Source/material.hpp
    Source/ray.hpp
    Source/sphere.hpp
    Source/vec3.hpp

    # Utility
    Source/rtweekend.hpp

    # Main
    Source/main.cpp

)

# Link and/or include our dependencies
target_include_directories(rtiow PRIVATE ${Stb_INCLUDE_DIR})
