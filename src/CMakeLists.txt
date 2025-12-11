# Source library for demo-jvm

# add subdirectories here as the project grows
add_subdirectory(common)
add_subdirectory(class_loader)

add_executable(demo-jvm main.cpp)

target_link_libraries(demo-jvm PRIVATE 
    jvm_common
    jvm_classloader
)

# optional: smoking test
# if(BUILD_TESTING)
#     add_test(NAME SmokeTest_RunMain COMMAND java_cmd)
# endif()