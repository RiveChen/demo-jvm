#include <iostream>

#include "native_registry.h"
#include "runtime/operand_stack.h"

namespace jvm::engine {
void jvm_print_string(runtime::OperandStack& s) {
  auto* str = static_cast<std::string*>(s.popRef());  // interned std::string*
  std::cout << *str;
}

void registerBuiltins() {
  NativeRegistry::getSingleton().bind("tests/data/java/Foo.print.(Ljava/lang/String;)V",
                                      &jvm_print_string);
}
}  // namespace jvm::engine