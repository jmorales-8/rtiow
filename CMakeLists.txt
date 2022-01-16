cmake_minimum_required(VERSION 3.10)

# set the project name
project(rtiow)

# by default CMake selects "ALL_BUILD" as the startup project 
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
	PROPERTY VS_STARTUP_PROJECT rtiow)

# add the executable
add_executable(rtiow 

	Source/main.cpp
	Source/vec3.hpp
	Source/color.hpp
	Source/ray.hpp

)

