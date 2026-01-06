add_library(jvm_oops STATIC method_area.cpp constant_pool.cpp klass.cpp)
target_link_libraries(jvm_oops PUBLIC jvm_utilities)