cmake_minimum_required(VERSION 3.10.0)
project(Feature-Engine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF) # Prefer not to use compiler-specific extensions

# Automatically collect all .cpp files in the src/ directory and its subdirectories
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add the executable with the collected source files
add_executable(Feature-Engine
    ${SOURCES}
    src/glad.c # Add glad.c explicitly if it's not a .cpp file
)

# Add include directory for glad
target_include_directories(Feature-Engine PRIVATE 
    ${CMAKE_SOURCE_DIR}/Libraries/include
    "${CMAKE_SOURCE_DIR}/Header Files"
)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# Find OpenGL package
find_package(OpenGL REQUIRED)

# Link libraries
target_link_libraries(Feature-Engine
    glfw
    OpenGL::GL
)