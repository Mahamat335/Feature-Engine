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
    ${CMAKE_SOURCE_DIR}/src/glad.c # Add glad.c explicitly if it's not a .cpp file
)

# Add include directories
target_include_directories(Feature-Engine PRIVATE 
    ${CMAKE_SOURCE_DIR}/Libraries/include
    "${CMAKE_SOURCE_DIR}/Header Files"
    "${CMAKE_SOURCE_DIR}/Header Files/Rendering"
)

# Copy or link the resources folder to the build directory
if (UNIX)
    # Create a symbolic link for resources on Unix systems
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${CMAKE_SOURCE_DIR}/resources" "${CMAKE_BINARY_DIR}/resources")
elseif (WIN32)
    # Copy resources folder on Windows
    file(COPY "${CMAKE_SOURCE_DIR}/resources" DESTINATION "${CMAKE_BINARY_DIR}")
endif()

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