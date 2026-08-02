add_library(jvm_utilities STATIC descriptor.cpp)
demo_jvm_configure_target(jvm_utilities)
target_include_directories(jvm_utilities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
