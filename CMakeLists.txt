cmake_minimum_required(VERSION 3.0)
project(GameDevSS19)

# ==============================================================================
# global settings

# enable folders for visual studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# you can enable C++17 if you like
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# some dependencies want to build as shared libs
set(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)


# ==============================================================================
# Set bin dir
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin/Default)
else()
    set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BIN_DIR})


# ==============================================================================
# dependencies

# Add glm library
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE extern/glm)
target_compile_definitions(glm INTERFACE
    GLM_FORCE_CXX11
    GLM_ENABLE_EXPERIMENTAL
    GLM_FORCE_CTOR_INIT # 0.9.9 breaks a lot otherwise
)
if (GCC)
    # class-memaccess error not available before gcc 8 (new-ish warning that triggers a lot in glm)
    if(NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 8)
        target_compile_options(glm INTERFACE -Wno-error=class-memaccess)
    endif()
endif()

# Add CTracer lib
add_subdirectory(extern/ctracer)

# Add GLOW lib
add_subdirectory(extern/glow)

# Add GLFW lib (with disabled spam)
option(GLFW_BUILD_EXAMPLES "" OFF)
option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)
if(MSVC)
  option(BUILD_SHARED_LIBS "" OFF)
else()
  option(BUILD_SHARED_LIBS "" ON)
endif()
add_subdirectory(extern/glfw)

# Add ImGui
add_subdirectory(extern/imgui)

# Add PolyMesh
add_subdirectory(extern/polymesh)

# Add GLOW Extras lib
add_subdirectory(extern/glow-extras)

# Bullet
# (physics engine)
add_subdirectory(extern/bullet)


# Folder grouping
foreach(TARGET_NAME glfw ctracer glad glow glow-extras imgui lodepng stb polymesh BulletCollision BulletDynamics)
  set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER "Extern") 
endforeach()


# ===========================================================================================
# Create target
file(GLOB_RECURSE SOURCES
    "src/*.cc"
    "src/*.hh"
    "logic/*.cc"
    "logic/*.hh"
    "data/*.*sh"
    "data/*.glsl"
)
if(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
endif()
add_executable(${PROJECT_NAME} ${SOURCES})

# Add src/ inc dir
target_include_directories(${PROJECT_NAME} PUBLIC 
    src
    src/logic
    src/logic/components
    src/logic/actors
    src/logic/base
    src/render
    extern/tinygltf
    )

# Link libs
target_link_libraries(${PROJECT_NAME} PUBLIC 
    ctracer
    glow
    glow-extras
    polymesh
    imgui
    BulletCollision
    BulletDynamics
)

# Compile flags

if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC 
        /MP # multi-core compiling
    )
else()
    target_compile_options(${PROJECT_NAME} PUBLIC 
        -Wall # useful warnings
        -Wno-unused-variable # not a useful warning
    )
endif()
