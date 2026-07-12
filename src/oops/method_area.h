#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "classfile/class_file.h"
#include "classfile/class_loader.h"
#include "klass.h"

// namespace jvm::classfile {
// class ClassLoader;
// class ClassFile;
// }  // namespace jvm::classfile

namespace jvm::oops {

struct ClassIdentifierHash {
  std::size_t operator()(const std::pair<classfile::ClassLoader*, std::string>& p) const {
    return std::hash<void*>{}(p.first) ^ (std::hash<std::string>{}(p.second) << 1U);
  }
};

// Singleton
class MethodArea {
 public:
  using ClassIdentifier = std::pair<classfile::ClassLoader*, std::string>;
  using ClassData       = std::pair<std::unique_ptr<Klass>, std::unique_ptr<classfile::ClassFile>>;

  // Meyer's singleton
  static MethodArea& getSingleton() {
    static MethodArea instance;
    return instance;
  }

  void   addClass(ClassIdentifier identifier, ClassData class_data);
  Klass* getClass(const ClassIdentifier& identifier);
  bool   hasClass(const ClassIdentifier& identifier) const;
  void   reset() { classes_.clear(); }

  // modernize-use-equals-delete
  // Implements rule 12.5.1 to explicitly default or delete special member functions
  MethodArea(const MethodArea&)            = delete;
  MethodArea(MethodArea&&)                 = delete;
  MethodArea& operator=(const MethodArea&) = delete;
  MethodArea& operator=(MethodArea&&)      = delete;

 private:
  MethodArea()  = default;
  ~MethodArea() = default;

  std::unordered_map<ClassIdentifier, ClassData, ClassIdentifierHash> classes_;
};

}  // namespace jvm::oops