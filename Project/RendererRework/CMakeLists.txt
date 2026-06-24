
file(GLOB sources 
    "*.cpp"
    )
    
    
    add_executable(RendererRework ${sources}) 
    target_link_libraries(RendererRework Engine)
    
    
set_target_properties(RendererRework PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_custom_target(RunEveryTime ALL
    COMMAND "${CMAKE_CURRENT_LIST_DIR}/Shaders/compile.sh"
    COMMENT "Executing my mandatory post-make task..."
)