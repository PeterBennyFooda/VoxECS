# Set the project name
add_library(fast_noise INTERFACE)

# Specify the directory where the header file is located
target_include_directories(fast_noise INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


