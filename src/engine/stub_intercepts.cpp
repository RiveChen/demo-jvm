#include "stub_intercepts.h"

#include <iostream>

#include "runtime/operand_stack.h"

namespace jvm::engine {
namespace {
void objectInit(runtime::OperandStack& s) { s.popRef(); }

void systemOut(runtime::OperandStack& s) {
  static char kOut;
  s.pushRef(&kOut);
}

void printlnString(runtime::OperandStack& s) {
  auto* str = static_cast<std::string*>(s.popRef());
  s.popRef();
  std::cout << *str << '\n';
}
}  // namespace

void registerStubIntercepts() {
  auto& t = StubIntercepts::getSingleton();
  t.bind("java/lang/Object.<init> ()V", &objectInit);
  t.bind("java/lang/System.out Ljava/io/PrintStream;", &systemOut);
  t.bind("java/io/PrintStream.println (Ljava/lang/String;)V", &printlnString);
}
}  // namespace jvm::engine