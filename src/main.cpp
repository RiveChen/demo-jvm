/**
 * @file main.cpp
 * @brief Minimal JVM launcher entry point.
 *
 * usage: demo-jvm <classpath-dir> <fully.qualified.MainClass>
 *
 * Example:
 *   demo-jvm build/test_classes tests.data.java.HelloWorld
 *
 * Loads the specified class, finds its main method, and invokes the
 * interpreter loop. This is the program entry point.
 */

#include <iostream>
#include <string>
#include <vector>

#include "utilities/logger.hpp"

#include "classfile/class_loader.hpp"
#include "engine/interpreter.hpp"
#include "engine/stub_intercepts.hpp"
#include "oops/klass.hpp"
#include "oops/method.hpp"
#include "runtime/frame.hpp"
#include "runtime/thread.hpp"

/**
 * @brief Program entry point.
 *
 * Parses command-line arguments for classpath and main class name.
 * Loads the class, looks up `main([Ljava/lang/String;)V`, creates
 * a Thread with an initial frame, and starts the interpreter loop.
 *
 * @param argc Argument count.
 * @param argv Argument vector:
 *             argv[1] = classpath directory (default: ".")
 *             argv[2] = fully qualified main class (default: "tests.data.java.HelloWorld")
 * @return 0 on success, 1 on error (class not found or no main method).
 */
int main(int argc, char** argv) {
  const std::string classpath  = (argc > 1) ? argv[1] : ".";
  const std::string main_class = (argc > 2) ? argv[2] : "tests.data.java.HelloWorld";

  LOG_INFO("Starting demo-jvm: classpath=", classpath, ", main=", main_class);

  jvm::engine::registerStubIntercepts();

  std::vector<std::string>    classpath_list{classpath};
  jvm::classfile::ClassLoader loader(nullptr, classpath_list);

  jvm::oops::Klass* klass = loader.loadClass(main_class);
  if (klass == nullptr) {
    std::cerr << "Error: class not found: " << main_class << '\n';
    return 1;
  }

  jvm::oops::Method* entry = klass->findMethod("main", "([Ljava/lang/String;)V");
  if (entry == nullptr) {
    std::cerr << "Error: no main([Ljava/lang/String;)V in " << main_class << '\n';
    return 1;
  }

  jvm::runtime::Thread     thread;
  jvm::engine::Interpreter interpreter;

  // Entry frame for main; local 0 = args array (null for now).
  jvm::runtime::Frame frame(entry);
  frame.getLocalVariables().setRef(0, nullptr);
  thread.pushFrame(std::move(frame));

  interpreter.interpret(&thread);
  LOG_INFO("Interpretation finished");
  return 0;
}