include_directories("imgui/")

file(GLOB sources 
    "Sources/*.cpp",
    "imgui/*.cpp",
    "imgui/backends/imgui_impl_glfw.cpp",
    "imgui/backends/imgui_impl_vulkan.cpp",
    "imgui/misc/cpp/imgui_stdlib.cpp",
)

add_executable(Editor ${sources}) 
target_link_libraries(Editor Engine)
target_link_libraries(Editor assimp)

set_target_properties(Editor PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)