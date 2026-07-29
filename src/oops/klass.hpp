/**
 * @file klass.h
 * @brief Runtime representation of a loaded Java class.
 *
 * "Klass" is the HotSpot JVM term for the in-memory representation
 * of a Java class. After a class file is parsed by the class file
 * parser, a Klass object is constructed to represent the class
 * at runtime, holding its methods, fields, constant pool,
 * and class hierarchy information.
 */

#pragma once

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

#include "constant_pool.hpp"
#include "field.hpp"
#include "method.hpp"
#include "runtime/thread.hpp"
#include "utilities/access_flags.hpp"
#include "utilities/slot.hpp"

namespace jvm::classfile {
class ClassLoader;
class ClassFile;
class MethodInfo;
}  // namespace jvm::classfile

namespace jvm::oops {

class Object;

/**
 * @brief Runtime representation of a loaded Java class.
 *
 * Created by ClassLoader::defineClass() after parsing a .class file.
 * Holds the resolved constant pool, methods, fields, static variables,
 * and links to super class and interfaces.
 */
class Klass {
 public:
  enum ClassState : uint8_t {
    Allocated,
    Loaded,
    Linked,
    BeingInitialized,
    FullyInitialized,
    InitializationError,
  };

  /// @brief Construct a Klass from a parsed class file.
  /// @param class_file The parsed class file (ownership transferred).
  /// @param loader The class loader that loaded this class.
  explicit Klass(classfile::ClassFile* class_file, classfile::ClassLoader* loader);

  ClassState getState() const { return state_; }
  void markLoaded() {
    assert(state_ == Allocated);
    state_ = Loaded;
  }
  void markFullyInitialized() {
    assert(state_ == BeingInitialized);
    state_ = FullyInitialized;
  }

  /// @name Class Hierarchy
  ///@{
  classfile::ClassLoader* getClassLoader() const { return loader_; }
  void                    setSuperClass(Klass* super_class) { super_class_ = super_class; }
  Klass*                  getSuperClass() const { return super_class_; }
  void setInterface(U2 index, Klass* interface) { interfaces_.at(index) = interface; }
  const std::vector<Klass*>& getInterfaces() const { return interfaces_; }
  ///@}

  /// @name Runtime Constant Pool
  RuntimeConstantPool& getRuntimeConstantPool() { return constant_pool_; }

  /// @name Instance and Static Layout
  ///@{
  size_t getInstanceSlotCount() const { return instance_slot_count_; }
  size_t getStaticSlotCount() const { return static_slot_count_; }
  bool   isInstanceOf(Klass* target) const;
  ///@}

  /// @name Member Lookup
  ///@{
  Method* findMethod(const std::string& name, const std::string& descriptor);
  Field*  findField(const std::string& name, const std::string& descriptor);
  ///@}

  /// @name Static Field Access
  ///@{
  Slot& getStaticSlot(size_t index) { return statics_.at(index); }
  ///@}

  /// @brief The fully qualified class name (e.g. "java.lang.Object").
  const std::string& getName() const { return name_; }

  void link(classfile::ClassFile* cf, classfile::ClassLoader* loader);
  void initialize(runtime::Thread* thread);

 private:
  ClassState state_;

  classfile::ClassLoader* loader_;

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

  /// @name Linking Helpers
  ///@{
  void prepareRuntimeConstantPool(classfile::ClassFile* class_file);
  void prepareMethods(classfile::ClassFile* class_file);
  void prepareFieldsAndStatics(classfile::ClassFile* class_file);
  void linkSuperClass(classfile::ClassFile*, classfile::ClassLoader*);
  void linkInterfaces(classfile::ClassFile*, classfile::ClassLoader*);
  ///@}
};

}  // namespace jvm::oops