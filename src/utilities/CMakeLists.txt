add_library(jvm_utilities STATIC descriptor.cpp)
target_include_directories(jvm_utilities
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
