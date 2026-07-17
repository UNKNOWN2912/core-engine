find_package(Vulkan REQUIRED)



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

include_directories("$ENV{VULKAN_SDK}/Include/")
include_directories("Vendor/json/single_include/nlohmann/")

# add_subdirectory(Platform)
add_subdirectory(Vendor)


add_library(Engine STATIC ${sources} ${platformSource})

target_link_libraries(Engine nlohmann_json)
target_link_libraries(Engine assimp)
target_link_libraries(Engine glfw)
target_link_libraries(Engine Vulkan::Vulkan)

