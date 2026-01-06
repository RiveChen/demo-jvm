#pragma once

namespace jvm::runtime {
class Thread;
}  // namespace jvm::runtime

namespace jvm::engine {

class Interpreter {
 public:
  void interpret(runtime::Thread* thread);
};

}  // namespace jvm::engine