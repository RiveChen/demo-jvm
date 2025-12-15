#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "class_file.h"

namespace jvm::runtime {
class Klass;
}  // namespace jvm::runtime

namespace jvm::class_loader {

// class ClassFile;

class ClassLoader {
 private:
  ClassLoader*                                     parent_;
  std::vector<std::string>                         classpaths_;
  std::unordered_map<std::string, runtime::Klass*> cache_;

  std::optional<std::vector<std::byte>> readClassFile(const std::string& name);
  runtime::Klass* defineClass(std::unique_ptr<class_loader::ClassFile> class_file,
                              const std::string&                       name);
  void            linkSuperClass(runtime::Klass* klass);
  void            linkInterfaces(runtime::Klass* klass);

 public:
  explicit ClassLoader(ClassLoader* parent = nullptr, std::vector<std::string> classpaths = {})
    : parent_(parent), classpaths_(std::move(classpaths)) {}

  runtime::Klass* loadClass(const std::string& name);
};

}  // namespace jvm::class_loader