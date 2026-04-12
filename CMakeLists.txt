# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# if(UNIX)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
# endif()


project ("Game-Engine")

include_directories("Engine")
include_directories("Engine/Vendor/glm/")
include_directories("Engine/Vendor/stb/")

# Include sub-projects.
add_subdirectory ("Engine")
add_subdirectory ("Project")

add_definitions(-CORE_USING_GLFW=1)
