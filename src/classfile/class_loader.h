#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "class_file.h"

namespace jvm::oops {
class Klass;
}  // namespace jvm::oops

namespace jvm::classfile {

// class ClassFile;

class ClassLoader {
 private:
  ClassLoader*                                  parent_;
  std::vector<std::string>                      classpaths_;
  std::unordered_map<std::string, oops::Klass*> cache_;

  std::optional<std::vector<std::byte>> readClassFile(const std::string& name);
  oops::Klass* defineClass(std::unique_ptr<ClassFile> class_file, const std::string& name);
  void         linkSuperClass(oops::Klass* klass, classfile::ClassFile* cf);
  void         linkInterfaces(oops::Klass* klass, classfile::ClassFile* cf);

 public:
  explicit ClassLoader(ClassLoader* parent = nullptr, std::vector<std::string> classpaths = {})
    : parent_(parent), classpaths_(std::move(classpaths)) {}

  oops::Klass* loadClass(const std::string& name);
};

}  // namespace jvm::classfile