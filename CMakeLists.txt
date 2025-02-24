cmake_minimum_required(VERSION 3.10)
project(UV_MAPPING VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Specify only Debug and Release configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Available configurations" FORCE)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(OpenGL REQUIRED)

# Include FetchContent for downloading dependencies
include(FetchContent)

# Fetch GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Fetch GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)

# Fetch GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# Fetch Dear ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.89.9
)
FetchContent_MakeAvailable(imgui)

# Collect source files
set(renderer_SOURCE
    src/Renderer.cpp
)

set(renderer_HEADERS
    include/Renderer.h
)

set(mesh_SOURCE
    src/Mesh.cpp
)

set(mesh_HEADERS
    include/Mesh.h
)

set(texture_SOURCE
    src/Texture.cpp
)

set(texture_HEADERS
    include/Texture.h
)

set(shader_SOURCE
    src/Shader.cpp
)

set(shader_HEADERS
    include/Shader.h
)

# Add ImGui source files
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Organize source files into groups
source_group(src FILES main.cpp)

# Renderer groups
source_group(src/renderer FILES ${renderer_SOURCE})
source_group(include/renderer FILES ${renderer_HEADERS})

# Mesh groups
source_group(src/mesh FILES ${mesh_SOURCE})
source_group(include/mesh FILES ${mesh_HEADERS})

# Texture groups
source_group(src/texture FILES ${texture_SOURCE})
source_group(include/texture FILES ${texture_HEADERS})

# Shader groups
source_group(src/shader FILES ${shader_SOURCE})
source_group(include/shader FILES ${shader_HEADERS})

# Documentation groups
source_group(README FILES README.md)

# ImGui groups
source_group(src/imgui FILES ${IMGUI_SOURCES})

# Add shaders folder to the project
file(GLOB SHADER_FILES "shaders/*.glsl")
source_group("Shaders" FILES ${SHADER_FILES})

# Create executable
add_executable(${PROJECT_NAME} main.cpp
    ${renderer_HEADERS} ${renderer_SOURCE}
    ${mesh_HEADERS} ${mesh_SOURCE}
    ${texture_HEADERS} ${texture_SOURCE}
    ${shader_HEADERS} ${shader_SOURCE}
    ${IMGUI_SOURCES}
    ${SHADER_FILES}
    README.md
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${glad_SOURCE_DIR}/include
    ${glfw_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    glad
    glm
    ${OPENGL_LIBRARIES}
)

# Copy shader files to build directory
file(GLOB SHADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.glsl")
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SHADER_FILES}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)

# Set working directory for Visual Studio
if(MSVC)
    set_property(TARGET ${PROJECT_NAME} 
        PROPERTY VS_DEBUGGER_WORKING_DIRECTORY 
        ${CMAKE_SOURCE_DIR}
    )
    
    # Copy README to output directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/README.md
    )
endif()

# Install documentation
install(FILES README.md DESTINATION .)