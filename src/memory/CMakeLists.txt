add_library(jvm_memory STATIC heap.cpp)
target_link_libraries(jvm_memory PUBLIC jvm_oops)
