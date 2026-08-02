add_library(jvm_memory STATIC heap.cpp)
demo_jvm_enable_warnings(jvm_memory)
target_link_libraries(jvm_memory PUBLIC jvm_oops)
