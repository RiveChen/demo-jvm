# Source library for demo-jvm

# add subdirectories here as the project grows
add_subdirectory(utilities)
add_subdirectory(memory)
add_subdirectory(oops)
add_subdirectory(classfile)
add_subdirectory(runtime)
add_subdirectory(engine)

add_executable(demo-jvm main.cpp)

target_link_libraries(demo-jvm PRIVATE jvm_utilities jvm_classfile jvm_engine)

# optional: smoking test if(BUILD_TESTING) add_test(NAME SmokeTest_RunMain
# COMMAND java_cmd) endif()
