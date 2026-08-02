add_library(jvm_memory STATIC heap.cpp)
demo_jvm_configure_target(jvm_memory)
target_link_libraries(jvm_memory PUBLIC jvm_oops)
