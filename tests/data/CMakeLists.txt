# 1. find Java compiler
find_package(Java REQUIRED)
include(UseJava)

# 2. define input and output paths
set(JAVA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/java)
set(CLASS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/test_classes) # output directory for compiled classes

# 3. collect all .java files
file(GLOB JAVA_SOURCES "${JAVA_SRC_DIR}/*.java")

# 4. define compiler flags using Java 8
# source: 8, target: 8, generate debug information: none, encoding: UTF-8
set(JAVAC_FLAGS -source 8 -target 8 -g:none -encoding UTF-8)

# 5. create output directory
make_directory(${CLASS_OUTPUT_DIR})

# 6. create a custom command to compile them
# javac -d build/test_classes tests/test_data/java/*.java
add_custom_command(
    OUTPUT ${CLASS_OUTPUT_DIR}/timestamp # use timestamp file as a generation marker
    COMMAND ${Java_JAVAC_EXECUTABLE} ${JAVAC_FLAGS} -d ${CLASS_OUTPUT_DIR} ${JAVA_SOURCES}
    COMMAND ${CMAKE_COMMAND} -E touch ${CLASS_OUTPUT_DIR}/timestamp
    DEPENDS ${JAVA_SOURCES}
    COMMENT "Compiling test Java files..."
    VERBATIM
)

# 7. create a target, so that other test modules can depend on it
add_custom_target(compile_test_classes ALL
    DEPENDS ${CLASS_OUTPUT_DIR}/timestamp
)