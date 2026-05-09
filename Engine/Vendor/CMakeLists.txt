add_subdirectory(glfw)

set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_BUILD_MINIZIP ON)

add_subdirectory(assimp)
