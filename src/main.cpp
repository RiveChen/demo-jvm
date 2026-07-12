#include <iostream>
#include <string>
#include <vector>

#include "classfile/class_loader.h"
#include "engine/interpreter.h"
#include "engine/native_registry.h"
#include "engine/stub_intercepts.h"
#include "oops/klass.h"
#include "oops/method.h"
#include "runtime/frame.h"
#include "runtime/thread.h"

// Minimal JVM launcher.
// usage: demo-jvm <classpath-dir> <fully.qualified.MainClass>
//   e.g. demo-jvm build/test_classes tests.data.java.HelloWorld
int main(int argc, char** argv) {
  const std::string classpath  = (argc > 1) ? argv[1] : ".";
  const std::string main_class = (argc > 2) ? argv[2] : "tests.data.java.HelloWorld";

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

  // Entry frame for main; local 0 = args array (null for now, unused by hello world).
  jvm::runtime::Frame frame(entry);
  frame.getLocalVariables().setRef(0, nullptr);
  thread.pushFrame(std::move(frame));

  jvm::engine::registerBuiltinNatives();
  jvm::engine::registerStubIntercepts();

  interpreter.interpret(&thread);
  return 0;
}
