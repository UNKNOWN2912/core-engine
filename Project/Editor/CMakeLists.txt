include_directories("imgui/")

file(GLOB sources 
    "*.cpp",
    "imgui/*.cpp",
    "imgui/backends/imgui_impl_glfw.cpp",
    "imgui/backends/imgui_impl_vulkan.cpp",
)

add_executable(Editor ${sources}) 
target_link_libraries(Editor Engine)