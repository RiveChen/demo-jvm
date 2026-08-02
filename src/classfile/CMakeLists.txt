add_library(jvm_classfile STATIC class_file_parser.cpp constant_pool.cpp
                                 attributes.cpp members.cpp class_loader.cpp)
demo_jvm_configure_target(jvm_classfile)
target_link_libraries(
  jvm_classfile
  PUBLIC jvm_utilities
  PUBLIC jvm_oops)
