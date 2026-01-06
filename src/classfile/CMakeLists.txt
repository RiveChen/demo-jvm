add_library(jvm_classfile STATIC class_file_parser.cpp constant_pool.cpp attributes.cpp members.cpp class_loader.cpp)
target_include_directories(jvm_classfile PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(jvm_classfile PUBLIC jvm_utilities PRIVATE jvm_oops)