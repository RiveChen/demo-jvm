add_library(jvm_engine STATIC interpreter.cpp)
target_include_directories(jvm_engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(jvm_engine PRIVATE jvm_common PUBLIC jvm_runtime)