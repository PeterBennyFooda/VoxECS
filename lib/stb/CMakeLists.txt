# Set the project name
add_library(stb INTERFACE)

# Specify the directory where the header file is located
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


