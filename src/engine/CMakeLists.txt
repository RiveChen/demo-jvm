add_library(jvm_engine STATIC interpreter.cpp)
target_link_libraries(jvm_engine PUBLIC jvm_runtime jvm_memory)
