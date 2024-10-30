cmake_minimum_required(VERSION 3.28)
project(Voxel_Wasteland LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Include all .cpp files in the src/ directory
file(GLOB_RECURSE SRC "src/*.cpp")

# Include all .h files in the include/ directory
file(GLOB_RECURSE HEADERS "include/*.h")

# Create the executable
add_executable(main ${SRC} ${HEADERS})

# Specify the precompiled header for the target
target_precompile_headers(main PRIVATE "src/pch.h")

# Copy shader files to build directory
# file(COPY "src/shader" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})  #THIS DOESN'T OVERWRITE FILES!
add_custom_command(
    TARGET main                   # The target this command is associated with
    PRE_BUILD                     # Run before building the target
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/src/shader
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shader
    COMMENT "Copying and overwriting shader directory"
)

# Copy asset files to build directory
add_custom_command(
    TARGET main                   # The target this command is associated with
    PRE_BUILD                     # Run before building the target
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/asset
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/asset
    COMMENT "Copying and overwriting shader directory"
)


# IMPORTANT: This make it easier to include files from the include/ directory
target_include_directories(main PRIVATE include)

target_link_libraries(main PRIVATE sfml-graphics)
target_compile_features(main PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
        TARGET main
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
        VERBATIM)
endif()

add_compile_definitions(_USE_MATH_DEFINES)

# Add OpenGL
find_package(OpenGL REQUIRED)

# Add glew
add_subdirectory(lib/glew-2.2.0)

# Add glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(lib/glfw-3.4)

# Add glm (tried manully adding glm, but it didn't work)
include(FetchContent)
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)

# Add stb
add_subdirectory(lib/stb)

# Add fast noise
add_subdirectory(lib/FastNoiseLite)

# Add nlohmann_json
# Typically you don't care so much for a third party library's tests to be
# run from your own project's code.
set(JSON_BuildTests OFF CACHE INTERNAL "")

# If you only include this third party in PRIVATE source files, you do not
# need to install it when your main project gets installed.
# set(JSON_Install OFF CACHE INTERNAL "")

# Don't use include(nlohmann_json/CMakeLists.txt) since that carries with it
# unintended consequences that will break the build.  It's generally
# discouraged (although not necessarily well documented as such) to use
# include(...) for pulling in other CMake projects anyways.
add_subdirectory(lib/nlohmann_json)

# Link libraries
set(OPENGL_LIB OpenGL::GL libglew_static glfw glm::glm stb fast_noise nlohmann_json::nlohmann_json)
target_link_libraries(main PUBLIC ${OPENGL_LIB})

