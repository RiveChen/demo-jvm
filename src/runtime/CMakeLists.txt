add_library(jvm_runtime STATIC klass.cpp method_area.cpp constant_pool.cpp)
target_include_directories(jvm_runtime PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(jvm_runtime PRIVATE jvm_common)