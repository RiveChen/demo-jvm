add_library(jvm_engine STATIC interpreter.cpp native_registry.cpp
                              stub_intercepts.cpp)
demo_jvm_configure_target(jvm_engine)
target_link_libraries(jvm_engine PUBLIC jvm_runtime jvm_memory)
