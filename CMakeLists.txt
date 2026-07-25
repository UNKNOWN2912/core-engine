cmake_minimum_required(VERSION 3.20.0)

project(engine-core)

find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 23)

file(GLOB sources   
    "Core/*.cpp"
    "Maths/*.cpp" 
    "Renderer/*.cpp" 
    "Renderer/Vulkan/*.cpp" 
    "Assets/*.cpp" 
    "Profiler/*.cpp"
    "EntityComponentSystem/*.cpp"
)


if(UNIX)
file(GLOB platformSource 
    "Platform/Linux/*.cpp"
)
elseif(WIN32)
file(GLOB platformSource 
    "Platform/Windows/*.cpp"
)
endif()

include_directories("./")
include_directories("$ENV{VULKAN_SDK}/Include/")
include_directories("Vendor/json/single_include/nlohmann/")
include_directories("Vendor/freetype/include/freetype")
include_directories("Vendor/glm/")

# add_subdirectory(Platform)
add_subdirectory(Vendor)


add_library(${PROJECT_NAME} STATIC ${sources} ${platformSource})

target_link_libraries(${PROJECT_NAME} nlohmann_json)
target_link_libraries(${PROJECT_NAME} assimp)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} freetype)

