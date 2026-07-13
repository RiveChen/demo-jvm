/**
 * @file stub_intercepts.cpp
 * @brief Register all built-in stub intercept handlers.
 *
 * These handlers simulate JDK class behavior without needing the
 * actual JDK class files. Currently supports:
 * - java.lang.Object.\<init\>       (no-op constructor)
 * - java.lang.System.out          (static sentinel push)
 * - java.io.PrintStream.println   (std::cout delegation)
 */

#include "stub_intercepts.h"

#include <iostream>

#include "runtime/operand_stack.h"
#include "utilities/logger.h"

namespace jvm::engine {
namespace {
/// Simulate java.lang.Object.\<init\>()V — pop and discard the receiver.
void objectInit(runtime::OperandStack& s) { s.popRef(); }

/// Simulate java.lang.System.out — push a static sentinel reference.
void systemOut(runtime::OperandStack& s) {
  static char kOut;
  s.pushRef(&kOut);
}

/// Simulate java.io.PrintStream.println(String)V — print to stdout.
void printlnString(runtime::OperandStack& s) {
  auto* str = static_cast<std::string*>(s.popRef());
  s.popRef();
  std::cout << *str << '\n';
}
}  // namespace

void registerStubIntercepts() {
  auto& t = StubIntercepts::getSingleton();
  LOG_DEBUG("Registered stub: java.lang.Object.<init>");
  t.bind("java/lang/Object.<init> ()V", &objectInit);
  t.bind("java/lang/System.out Ljava/io/PrintStream;", &systemOut);
  t.bind("java/io/PrintStream.println (Ljava/lang/String;)V", &printlnString);
}
}  // namespace jvm::engine