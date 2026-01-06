add_library(jvm_interpreter STATIC interpreter.cpp)
target_include_directories(jvm_interpreter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(jvm_interpreter PRIVATE jvm_utilities PUBLIC jvm_runtime)