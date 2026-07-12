add_library(jvm_engine STATIC interpreter.cpp)
target_link_libraries(
  jvm_engine
  PRIVATE jvm_utilities
  PUBLIC jvm_runtime)
