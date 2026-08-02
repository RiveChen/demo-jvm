add_library(jvm_oops STATIC method_area.cpp constant_pool.cpp klass.cpp)
demo_jvm_enable_warnings(jvm_oops)
target_link_libraries(jvm_oops PUBLIC jvm_utilities)
