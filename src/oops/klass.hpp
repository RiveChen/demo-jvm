/**
 * @file klass.hpp
 * @brief Runtime representation of a loaded Java class (Klass hierarchy).
 *
 * Mirrors HotSpot's klass-oop split: Klass (metadata, in MethodArea)
 * vs OopDesc (object header, on heap).
 *
 * Klass hierarchy:
 *   Klass (base: name, super_class, kind, state)
 *   ├── InstanceKlass (from .class files: methods, fields, constant pool)
 *   ├── TypeArrayKlass (primitive arrays: int[], byte[], ...)
 *   └── ObjArrayKlass (reference arrays: Object[], String[], ...)
 */

#pragma once

#include "constant_pool.hpp"
#include "field.hpp"
#include "method.hpp"
#include "runtime/thread.hpp"
#include "utilities/access_flags.hpp"
#include "utilities/basic_type.hpp"
#include "utilities/slot.hpp"
#include "utilities/types.hpp"

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

namespace jvm::classfile {
class ClassLoader;
class ClassFile;
class MethodInfo;
}  // namespace jvm::classfile

namespace jvm::oops {

class OopDesc;

// ============================================================================
//  Klass  —  abstract base of the Klass hierarchy
// ============================================================================

class Klass {
 public:
  enum class Kind : uint8_t { Instance, TypeArray, ObjArray };
  enum ClassState : uint8_t {
    Allocated,
    Loaded,
    Linked,
    BeingInitialized,
    FullyInitialized,
    InitializationError,
  };

 protected:
  Kind                      kind_;
  ClassState                state_;
  std::string               name_;
  AccessFlags<flags::Class> access_flags_;
  Klass*                    super_class_;

  Klass(Kind kind, std::string name, AccessFlags<flags::Class> flags)
    : kind_(kind), state_(Allocated), name_(std::move(name)), access_flags_(flags), super_class_(nullptr) {}

 public:
  virtual ~Klass() = default;

  Kind       kind() const { return kind_; }
  ClassState getState() const { return state_; }

  void markLoaded() {
    assert(state_ == Allocated);
    state_ = Loaded;
  }
  void markFullyInitialized() {
    assert(state_ == BeingInitialized);
    state_ = FullyInitialized;
  }

  // -- Virtual interface (overridden by InstanceKlass / ArrayKlass) --

  virtual const std::string&      getName() const { return name_; }
  virtual Klass*                  getSuperClass() const { return super_class_; }
  virtual classfile::ClassLoader* getClassLoader() const                  = 0;
  virtual bool                    isInstanceOf(const Klass* target) const = 0;
  virtual std::string             getDescriptorName();
};

// ============================================================================
//  InstanceKlass  —  a regular class loaded from a .class file
// ============================================================================

class InstanceKlass : public Klass {
 public:
  explicit InstanceKlass(classfile::ClassFile* class_file, classfile::ClassLoader* loader);

  // -- Klass virtual overrides --
  classfile::ClassLoader* getClassLoader() const override { return loader_; }
  bool                    isInstanceOf(const Klass* target) const override;

  // -- Class hierarchy (well-typed for InstanceKlass) --
  void                               setSuperClass(InstanceKlass* super) { super_class_ = super; }
  void                               setInterface(U2 index, InstanceKlass* iface) { interfaces_.at(index) = iface; }
  const std::vector<InstanceKlass*>& getInterfaces() const { return interfaces_; }

  // -- Runtime constant pool --
  RuntimeConstantPool& getRuntimeConstantPool() { return constant_pool_; }

  // -- Slot layout --
  size_t getInstanceSlotCount() const { return instance_slot_count_; }
  size_t getStaticSlotCount() const { return static_slot_count_; }
  Slot&  getStaticSlot(size_t index) { return statics_.at(index); }

  // -- Member lookup --
  Method* findMethod(const std::string& name, const std::string& descriptor, bool find_in_super = true);
  Field*  findField(const std::string& name, const std::string& descriptor);

  // -- Linking & initialization --
  void link(classfile::ClassFile* cf, classfile::ClassLoader* loader);
  void initialize(runtime::Thread* thread);

 private:
  classfile::ClassLoader* loader_{nullptr};

  std::vector<InstanceKlass*> interfaces_;
  RuntimeConstantPool         constant_pool_;
  std::vector<Method>         methods_;
  std::vector<Field>          fields_;
  std::vector<Slot>           statics_;

  size_t instance_slot_count_{};
  size_t static_slot_count_{};

  OopDesc* mirror_class_object_{nullptr};

  // -- Linking helpers --
  void prepareRuntimeConstantPool(classfile::ClassFile* class_file);
  void prepareMethods(classfile::ClassFile* class_file);
  void prepareFieldsAndStatics(classfile::ClassFile* class_file);
  void linkSuperClass(classfile::ClassFile*, classfile::ClassLoader*);
  void linkInterfaces(classfile::ClassFile*, classfile::ClassLoader*);
};

// ============================================================================
//  ArrayKlass  —  intermediate base for array classes
// ============================================================================

class ArrayKlass : public Klass {
 protected:
  Jint element_size_{0};

  ArrayKlass(Kind kind, std::string name, Jint elem_size, AccessFlags<flags::Class> flags)
    : Klass(kind, std::move(name), flags), element_size_(elem_size) {}

 public:
  Jint                    elementSize() const { return element_size_; }
  classfile::ClassLoader* getClassLoader() const override { return nullptr; }
};

// ============================================================================
//  TypeArrayKlass  —  primitive arrays (int[], byte[], ...)
// ============================================================================

class TypeArrayKlass : public ArrayKlass {
 private:
  BasicType type_;

 public:
  TypeArrayKlass(BasicType type, Jint elem_size, std::string name)
    : ArrayKlass(Klass::Kind::TypeArray, std::move(name), elem_size,
                 AccessFlags<flags::Class>(flags::Class::PUBLIC | flags::Class::FINAL | flags::Class::ABSTRACT)),
      type_(type) {
    // Arrays are always fully initialized immediately
    state_ = FullyInitialized;
  }

  bool isInstanceOf(const Klass* target) const override;
};

// ============================================================================
//  ObjArrayKlass  —  reference arrays (Object[], String[], ...)
// ============================================================================

class ObjArrayKlass : public ArrayKlass {
  Klass* element_klass_{nullptr};

 public:
  ObjArrayKlass(Klass* element, std::string name)
    : ArrayKlass(Klass::Kind::ObjArray, std::move(name), static_cast<Jint>(sizeof(void*)),
                 AccessFlags<flags::Class>(flags::Class::PUBLIC | flags::Class::FINAL | flags::Class::ABSTRACT)),
      element_klass_(element) {
    // Arrays are always fully initialized immediately
    state_ = FullyInitialized;
  }

  Klass* elementKlass() const { return element_klass_; }
  bool   isInstanceOf(const Klass* target) const override;
};

}  // namespace jvm::oops