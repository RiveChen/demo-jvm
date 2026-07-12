add_library(jvm_classfile STATIC class_file_parser.cpp constant_pool.cpp
                                 attributes.cpp members.cpp class_loader.cpp)
target_link_libraries(
  jvm_classfile
  PUBLIC jvm_utilities
  PUBLIC jvm_oops)
