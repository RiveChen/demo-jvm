add_library(jvm_classloader STATIC class_file_parser.cpp constant_pool.cpp attributes.cpp)
target_include_directories(jvm_classloader PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(jvm_classloader PUBLIC jvm_common)