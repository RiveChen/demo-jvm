# Test configuration
# GoogleTest module is available in CMake 3.20+ and works with FetchContent
include(GoogleTest)

# Add subdirectories here as the project grows
add_subdirectory(data)
add_subdirectory(sanity)
add_subdirectory(modules)