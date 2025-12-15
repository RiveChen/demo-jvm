#pragma once

#include <vector>

#include "common/access_flags.hpp"
#include "constant_pool.h"
#include "field.h"
#include "method.h"
#include "slot.h"

namespace jvm::class_loader {
class ClassLoader;
class ClassFile;
class MethodInfo;
}  // namespace jvm::class_loader

namespace jvm::runtime {

class Object;

// enum class KlassState {
//   LOADED,
//   LINKED,
//   PREPARING,
//   INITIALIZING,
//   INITIALIZED,
//   ERROR,
// };

class Klass {
 public:
  explicit Klass(class_loader::ClassFile* class_file, class_loader::ClassLoader* loader);

  class_loader::ClassLoader* getClassLoader() const { return loader_; }
  class_loader::ClassFile*   getClassFile() const { return class_file_; }
  void                       setSuperClass(Klass* super_class) { super_class_ = super_class; }
  Klass*                     getSuperClass() const { return super_class_; }
  void setInterface(U2 index, Klass* interface) { interfaces_[index] = interface; }
  const std::vector<Klass*>& getInterfaces() const { return interfaces_; }
  RuntimeConstantPool&       getRuntimeConstantPool() { return constant_pool_; }
  size_t                     getInstanceSlotCount() const { return instance_slot_count_; }
  size_t                     getStaticSlotCount() const { return static_slot_count_; }
  Method*                    findMethod(const std::string& name, const std::string& descriptor);
  Field*                     findField(const std::string& name, const std::string& descriptor);
  Slot&                      getStaticSlot(size_t index) { return statics_[index]; }

 private:
  class_loader::ClassLoader* loader_;
  class_loader::ClassFile*
    class_file_;  // class_file should be initialized first for constant_pool to be valid

  std::string               name_;
  AccessFlags<flags::Class> access_flags_;
  Klass*                    super_class_;
  std::vector<Klass*>       interfaces_;
  RuntimeConstantPool       constant_pool_;
  std::vector<Method>       methods_;
  std::vector<Field>        fields_;
  std::vector<Slot>         statics_;

  size_t instance_slot_count_{};
  size_t static_slot_count_{};

  Object* mirror_class_object_;

  // define class
  void prepareRuntimeConstantPool(class_loader::ClassFile* class_file);
  void prepareMethods(class_loader::ClassFile* class_file);
  void prepareFieldsAndStatics(class_loader::ClassFile* class_file);
  // void linkNativeMethods(runtime::Method* method);

  friend class class_loader::ClassLoader;
};

}  // namespace jvm::runtime