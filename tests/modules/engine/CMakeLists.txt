# Test base header path
set(TEST_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Arithmetic tests (existing)
add_executable(test_interpreter_arithmetic interpreter_arithmetic_test.cpp)
target_link_libraries(test_interpreter_arithmetic
                      PRIVATE jvm_engine jvm_classfile GTest::gtest_main)
target_include_directories(test_interpreter_arithmetic PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_arithmetic compile_test_classes)
target_compile_definitions(
  test_interpreter_arithmetic
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_arithmetic
                     PROPERTIES LABELS "spec;regression")

# Constants tests
add_executable(test_interpreter_constants interpreter_constants_test.cpp)
target_link_libraries(test_interpreter_constants
                      PRIVATE jvm_engine jvm_classfile GTest::gtest_main)
target_include_directories(test_interpreter_constants PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_constants compile_test_classes)
target_compile_definitions(
  test_interpreter_constants
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_constants
                     PROPERTIES LABELS "spec;regression")

# Fail-fast contract for every opcode that is not implemented yet
add_executable(test_interpreter_unimplemented_opcode
               interpreter_unimplemented_opcode_test.cpp)
target_link_libraries(test_interpreter_unimplemented_opcode
                      PRIVATE jvm_engine jvm_classfile GTest::gtest_main)
target_include_directories(test_interpreter_unimplemented_opcode
                           PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_unimplemented_opcode compile_test_classes)
target_compile_definitions(
  test_interpreter_unimplemented_opcode
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_unimplemented_opcode
                     PROPERTIES LABELS "regression")

# Load/Store tests
add_executable(test_interpreter_load_store interpreter_load_store_test.cpp)
target_link_libraries(test_interpreter_load_store
                      PRIVATE jvm_engine jvm_classfile GTest::gtest_main)
target_include_directories(test_interpreter_load_store PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_load_store compile_test_classes)
target_compile_definitions(
  test_interpreter_load_store
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_load_store PROPERTIES LABELS "spec")

# Stack operation tests
add_executable(test_interpreter_stack interpreter_stack_test.cpp)
target_link_libraries(test_interpreter_stack PRIVATE jvm_engine jvm_classfile
                                                     GTest::gtest_main)
target_include_directories(test_interpreter_stack PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_stack compile_test_classes)
target_compile_definitions(
  test_interpreter_stack
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_stack PROPERTIES LABELS "spec")

# Conversion tests
add_executable(test_interpreter_conversion interpreter_conversion_test.cpp)
target_link_libraries(test_interpreter_conversion
                      PRIVATE jvm_engine jvm_classfile GTest::gtest_main)
target_include_directories(test_interpreter_conversion PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_conversion compile_test_classes)
target_compile_definitions(
  test_interpreter_conversion
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_conversion
                     PROPERTIES LABELS "spec;regression")

# Control flow tests
add_executable(test_interpreter_control_flow interpreter_control_flow_test.cpp)
target_link_libraries(test_interpreter_control_flow
                      PRIVATE jvm_engine jvm_classfile GTest::gtest_main)
target_include_directories(test_interpreter_control_flow
                           PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_control_flow compile_test_classes)
target_compile_definitions(
  test_interpreter_control_flow
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_control_flow PROPERTIES LABELS "spec")

# Method invocation tests
add_executable(test_interpreter_method_invocation
               interpreter_method_invocation_test.cpp)
target_link_libraries(test_interpreter_method_invocation
                      PRIVATE jvm_engine jvm_classfile GTest::gtest_main)
target_include_directories(test_interpreter_method_invocation
                           PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_method_invocation compile_test_classes)
target_compile_definitions(
  test_interpreter_method_invocation
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_method_invocation
                     PROPERTIES LABELS "spec")

# Fields tests (GETSTATIC, PUTSTATIC)
add_executable(test_interpreter_fields interpreter_fields_test.cpp)
target_link_libraries(test_interpreter_fields PRIVATE jvm_engine jvm_classfile
                                                      GTest::gtest_main)
target_include_directories(test_interpreter_fields PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_fields compile_test_classes)
target_compile_definitions(
  test_interpreter_fields
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_fields PROPERTIES LABELS "spec")

# Object tests (NEW, INVOKESPECIAL <init>, GETFIELD, PUTFIELD)
add_executable(test_interpreter_object interpreter_object_test.cpp)
target_link_libraries(test_interpreter_object PRIVATE jvm_engine jvm_classfile
                                                      GTest::gtest_main)
target_include_directories(test_interpreter_object PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_object compile_test_classes)
target_compile_definitions(
  test_interpreter_object
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_object PROPERTIES LABELS "spec")

# Native method dispatch tests (INVOKESTATIC of ACC_NATIVE -> NativeRegistry)
add_executable(test_interpreter_native interpreter_native_test.cpp)
target_link_libraries(test_interpreter_native PRIVATE jvm_engine jvm_classfile
                                                      GTest::gtest_main)
target_include_directories(test_interpreter_native PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_native compile_test_classes)
target_compile_definitions(
  test_interpreter_native
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_native PROPERTIES LABELS "integration")

# End-to-end stubbed hello world (GETSTATIC System.out + LDC + INVOKEVIRTUAL println)
add_executable(test_interpreter_hello_world interpreter_hello_world_test.cpp)
target_link_libraries(test_interpreter_hello_world PRIVATE jvm_engine jvm_classfile
                                                           GTest::gtest_main)
target_include_directories(test_interpreter_hello_world PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_hello_world compile_test_classes)
target_compile_definitions(
  test_interpreter_hello_world
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_hello_world
                     PROPERTIES LABELS "integration")

# Tier 1 stub intercept tests (print/println, String methods, static utils)
add_executable(test_interpreter_stub_intercept interpreter_stub_intercept_test.cpp)
target_link_libraries(test_interpreter_stub_intercept
                      PRIVATE jvm_engine jvm_classfile GTest::gtest_main)
target_include_directories(test_interpreter_stub_intercept PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_stub_intercept compile_test_classes)
target_compile_definitions(
  test_interpreter_stub_intercept
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_stub_intercept
                     PROPERTIES LABELS "integration")

# INVOKEINTERFACE tests
add_executable(test_interpreter_interface_invoke interpreter_interface_invoke_test.cpp)
target_link_libraries(test_interpreter_interface_invoke
                      PRIVATE jvm_engine jvm_classfile GTest::gtest_main)
target_include_directories(test_interpreter_interface_invoke PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_interface_invoke compile_test_classes)
target_compile_definitions(
  test_interpreter_interface_invoke
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_interface_invoke
                     PROPERTIES LABELS "spec")

# <clinit> class initialization tests
add_executable(test_interpreter_clinit interpreter_clinit_test.cpp)
target_link_libraries(test_interpreter_clinit PRIVATE jvm_engine jvm_classfile
                                                          GTest::gtest_main)
target_include_directories(test_interpreter_clinit PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_clinit compile_test_classes)
target_compile_definitions(
  test_interpreter_clinit
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_clinit PROPERTIES LABELS "spec")

# Init/type-check regression tests (B1 pc-reset, B2 super-interface instanceof,
# B3 inherited <clinit>, B4 super/sub <clinit> order)
add_executable(test_interpreter_init_type_regression
               interpreter_init_type_regression_test.cpp)
target_link_libraries(test_interpreter_init_type_regression
                      PRIVATE jvm_engine jvm_classfile GTest::gtest_main)
target_include_directories(test_interpreter_init_type_regression PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_init_type_regression compile_test_classes)
target_compile_definitions(
  test_interpreter_init_type_regression
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_init_type_regression
                     PROPERTIES LABELS "spec;regression")

# CHECKCAST / INSTANCEOF tests
add_executable(test_interpreter_type_check interpreter_type_check_test.cpp)
target_link_libraries(test_interpreter_type_check
                      PRIVATE jvm_engine jvm_classfile GTest::gtest_main)
target_include_directories(test_interpreter_type_check PRIVATE ${TEST_BASE_DIR})
add_dependencies(test_interpreter_type_check compile_test_classes)
target_compile_definitions(
  test_interpreter_type_check
  PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")
gtest_discover_tests(test_interpreter_type_check PROPERTIES LABELS "spec")
