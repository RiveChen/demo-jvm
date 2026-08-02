# Test base header path
set(TEST_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

function(add_interpreter_test name)
  cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "LABELS")

  if(NOT ARG_LABELS)
    message(FATAL_ERROR
            "add_interpreter_test(${name}) requires at least one LABELS value")
  endif()

  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
            "Unknown arguments for ${name}: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  set(target "test_interpreter_${name}")

  add_executable(${target} "interpreter_${name}_test.cpp")
  demo_jvm_configure_target(${target})

  target_link_libraries(
    ${target}
    PRIVATE jvm_engine
            jvm_classfile
            GTest::gtest_main)

  target_include_directories(
    ${target}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

  add_dependencies(${target} compile_test_classes)

  target_compile_definitions(
    ${target}
    PRIVATE TEST_CLASS_PATH="${CMAKE_BINARY_DIR}/test_classes")

  gtest_discover_tests(
    ${target}
    PROPERTIES LABELS "${ARG_LABELS}")
endfunction()

add_interpreter_test(arithmetic LABELS spec regression)
add_interpreter_test(entry_return LABELS spec regression)
add_interpreter_test(constants LABELS spec regression)
add_interpreter_test(unimplemented_opcode LABELS regression)
add_interpreter_test(load_store LABELS spec)
add_interpreter_test(stack LABELS spec)
add_interpreter_test(conversion LABELS spec regression)
add_interpreter_test(control_flow LABELS spec)
add_interpreter_test(method_invocation LABELS spec)
add_interpreter_test(fields LABELS spec)
add_interpreter_test(object LABELS spec)
add_interpreter_test(native LABELS integration)
add_interpreter_test(hello_world LABELS integration)
add_interpreter_test(stub_intercept LABELS integration)
add_interpreter_test(interface_invoke LABELS spec)
add_interpreter_test(clinit LABELS spec)
add_interpreter_test(init_type_regression LABELS spec regression)
add_interpreter_test(type_check LABELS spec)
