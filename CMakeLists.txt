# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)

project ("Game-Engine")

include_directories("Engine")

# Include sub-projects.
add_subdirectory ("Engine")
add_subdirectory ("Project")
