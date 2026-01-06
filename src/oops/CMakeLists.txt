add_library(jvm_oops STATIC method_area.cpp constant_pool.cpp klass.cpp)
target_include_directories(jvm_oops PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(jvm_oops PRIVATE jvm_utilities)