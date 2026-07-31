/**
 * @file klass.cpp
 * @brief InstanceKlass construction, linking, and ArrayKlass isInstanceOf.
 */

#include "klass.hpp"

#include "classfile/class_file.hpp"
#include "classfile/class_loader.hpp"
#include "classfile/constant_pool.hpp"
#include "classfile/members.hpp"
#include "constant_pool.hpp"
#include "runtime/frame.hpp"
#include "utilities/access_flags.hpp"
#include "utilities/logger.hpp"
#include "utilities/slot.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace jvm::oops {

std::string Klass::getDescriptorName() {
  if (kind_ == Kind::Instance) {
    return "L" + name_ + ";";
  }
  return name_;
}

// ============================================================================
// InstanceKlass
// ============================================================================

InstanceKlass::InstanceKlass(classfile::ClassFile* class_file, classfile::ClassLoader* loader)
  : Klass(Klass::Kind::Instance,
          class_file->constant_pool.getClassName(class_file->this_class_index),
          class_file->access_flags),
    loader_(loader),
    interfaces_(class_file->interfaces_count),
    constant_pool_(this) {}

void InstanceKlass::link(classfile::ClassFile* cf, classfile::ClassLoader* loader) {
  linkSuperClass(cf, loader);
  linkInterfaces(cf, loader);
  prepareRuntimeConstantPool(cf);
  prepareMethods(cf);
  prepareFieldsAndStatics(cf);
  state_ = Linked;
}

void InstanceKlass::initialize(runtime::Thread* thread) {
  if (state_ == FullyInitialized) {
    return;
  }
  if (state_ == BeingInitialized) {
    return;
  }
  if (state_ == InitializationError) {
    throw std::runtime_error("NoClassDefFoundError: initialization error");
  }

  state_         = BeingInitialized;
  Method* clinit = findMethod("<clinit>", "()V", false);
  if (clinit != nullptr) {
    // push its own <clinit> method
    runtime::Frame frame(clinit);
    thread->pushFrame(std::move(frame));
  } else {
    // a subclass without its own <clinit> should also initializes its
    // superclass
    state_ = FullyInitialized;
  }

  // super's frame first
  if (super_class_ != nullptr) {
    static_cast<InstanceKlass*>(super_class_)->initialize(thread);
  }
}

Method* InstanceKlass::findMethod(const std::string& name, const std::string& descriptor,
                                  bool find_in_super) {
  for (auto& m : methods_) {
    if (m.getName() == name && m.getDescriptor() == descriptor) {
      return &m;
    }
  }
  if (find_in_super && super_class_ != nullptr) {
    return static_cast<InstanceKlass*>(super_class_)->findMethod(name, descriptor);
  }
  return nullptr;
}

Field* InstanceKlass::findField(const std::string& name, const std::string& descriptor) {
  for (auto& f : fields_) {
    if (f.getName() == name && f.getDescriptor() == descriptor) {
      return &f;
    }
  }
  if (super_class_ != nullptr) {
    return static_cast<InstanceKlass*>(super_class_)->findField(name, descriptor);
  }
  return nullptr;
}

bool InstanceKlass::isInstanceOf(const Klass* target) const {
  if (this == target) {
    return true;
  }
  if (super_class_ != nullptr && super_class_->isInstanceOf(target)) {
    return true;
  }

  for (const auto* iface : interfaces_) {
    if (iface != nullptr && iface->isInstanceOf(target)) {
      return true;
    }
  }
  return false;
}

// ============================================================================
//  Linking helpers
// ============================================================================

void InstanceKlass::prepareRuntimeConstantPool(classfile::ClassFile* class_file) {
  LOG_DEBUG("Preparing runtime constant pool for ", name_, " (", class_file->constant_pool.size(),
            " entries)");
  size_t cp_count = class_file->constant_pool.size();
  constant_pool_.infos_.resize(cp_count);
  constant_pool_.infos_[0] = std::monostate{};
  for (U2 i = 1; i < cp_count; i++) {
    const auto* cpinfo = class_file->constant_pool.getConstantInfo(i);
    switch (cpinfo->tag) {
      case classfile::ConstantTag::kClass: {
        const auto* info = static_cast<const classfile::ClassInfo*>(cpinfo);
        constant_pool_.setConstant(
          i, SymRef_Class{.class_name = class_file->constant_pool.getUtf8String(info->name_index)});
      } break;
      case classfile::ConstantTag::kMethodref: {
        const auto* info   = static_cast<const classfile::MethodrefInfo*>(cpinfo);
        auto name_and_type = class_file->constant_pool.getNameAndType(info->name_and_type_index);
        constant_pool_.setConstant(i, SymRef_Method{.class_cp_index = info->class_index,
                                                    .member_name    = name_and_type.first,
                                                    .descriptor     = name_and_type.second});
      } break;
      case classfile::ConstantTag::kFieldref: {
        const auto* info   = static_cast<const classfile::FieldrefInfo*>(cpinfo);
        auto name_and_type = class_file->constant_pool.getNameAndType(info->name_and_type_index);
        constant_pool_.setConstant(i, SymRef_Field{.class_cp_index = info->class_index,
                                                   .member_name    = name_and_type.first,
                                                   .descriptor     = name_and_type.second});
      } break;
      case classfile::ConstantTag::kInterfaceMethodref: {
        const auto* info   = static_cast<const classfile::InterfaceMethodrefInfo*>(cpinfo);
        auto name_and_type = class_file->constant_pool.getNameAndType(info->name_and_type_index);
        constant_pool_.setConstant(i, SymRef_InterfaceMethod{.class_cp_index = info->class_index,
                                                             .member_name    = name_and_type.first,
                                                             .descriptor = name_and_type.second});
      } break;
      case classfile::ConstantTag::kInteger: {
        constant_pool_.setConstant(i, static_cast<const classfile::IntegerInfo*>(cpinfo)->value);
      } break;
      case classfile::ConstantTag::kLong: {
        constant_pool_.setConstant(i, static_cast<const classfile::LongInfo*>(cpinfo)->value);
        i++;
        constant_pool_.infos_[i] = std::monostate{};
      } break;
      case classfile::ConstantTag::kFloat: {
        constant_pool_.setConstant(i, static_cast<const classfile::FloatInfo*>(cpinfo)->value);
      } break;
      case classfile::ConstantTag::kDouble: {
        constant_pool_.setConstant(i, static_cast<const classfile::DoubleInfo*>(cpinfo)->value);
        i++;
        constant_pool_.infos_[i] = std::monostate{};
      } break;
      case classfile::ConstantTag::kString: {
        auto index     = static_cast<const classfile::StringInfo*>(cpinfo)->string_index;
        auto rt_cpinfo = class_file->constant_pool.getUtf8String(index);
        constant_pool_.setConstant(i, rt_cpinfo);
      } break;
      case classfile::ConstantTag::kMethodHandle:
      case classfile::ConstantTag::kMethodType:
      case classfile::ConstantTag::kInvokeDynamic:
      case classfile::ConstantTag::kNameAndType:
      case classfile::ConstantTag::kUtf8:
        break;
      default:
        throw std::runtime_error("Unknown constant pool tag: " +
                                 std::to_string(static_cast<int>(cpinfo->tag)));
    }
  }
}

void InstanceKlass::prepareMethods(classfile::ClassFile* class_file) {
  LOG_DEBUG("Preparing methods for class ", name_);
  for (auto& member_info : class_file->methods.getMembers()) {
    auto*  method_info  = dynamic_cast<classfile::MethodInfo*>(member_info.get());
    auto   access_flags = method_info->access_flags;
    auto   name         = class_file->constant_pool.getUtf8String(method_info->name_index);
    auto   descriptor   = class_file->constant_pool.getUtf8String(method_info->descriptor_index);
    Method method(access_flags, name, descriptor, this);
    if (access_flags.has(flags::Method::NATIVE)) {
      // TODO: native method binding
    } else if (!access_flags.has(flags::Method::ABSTRACT)) {
      auto* code_attribute = method_info->attributes.findAttribute<classfile::CodeAttribute>();
      if (code_attribute != nullptr) {
        method.max_stack_  = code_attribute->max_stack;
        method.max_locals_ = code_attribute->max_locals;
        method.code_       = std::move(code_attribute->code);
      } else {
        throw std::runtime_error("Method " + name + " has no code attribute");
      }
    }
    methods_.emplace_back(std::move(method));
  }
}

void InstanceKlass::prepareFieldsAndStatics(classfile::ClassFile* class_file) {
  size_t instance_slot_count = (super_class_ != nullptr)
                                 ? static_cast<InstanceKlass*>(super_class_)->getInstanceSlotCount()
                                 : 0;
  size_t static_slot_count   = 0;
  std::vector<std::pair<size_t, Slot>> static_inits;
  for (auto& member_info : class_file->fields.getMembers()) {
    auto* field_info   = dynamic_cast<classfile::FieldInfo*>(member_info.get());
    auto  access_flags = field_info->access_flags;
    auto  name         = class_file->constant_pool.getUtf8String(field_info->name_index);
    auto  descriptor   = class_file->constant_pool.getUtf8String(field_info->descriptor_index);
    Field field(access_flags, name, descriptor, this);
    if (access_flags.has(flags::Field::STATIC)) {
      field.slot_index_ = static_slot_count;
      static_slot_count += (descriptor == "J" || descriptor == "D") ? 2 : 1;
      const auto* cv = field_info->attributes.findAttribute<classfile::ConstantValueAttribute>();
      if (cv != nullptr) {
        const auto* cp_info = class_file->constant_pool.getConstantInfo(cv->value);
        switch (cp_info->tag) {
          case classfile::ConstantTag::kInteger:
            static_inits.emplace_back(
              field.slot_index_,
              Slot{.i = static_cast<const classfile::IntegerInfo*>(cp_info)->value});
            break;
          case classfile::ConstantTag::kFloat:
            static_inits.emplace_back(
              field.slot_index_,
              Slot{.f = static_cast<const classfile::FloatInfo*>(cp_info)->value});
            break;
          case classfile::ConstantTag::kLong:
            static_inits.emplace_back(
              field.slot_index_,
              Slot{.l = static_cast<const classfile::LongInfo*>(cp_info)->value});
            static_inits.emplace_back(field.slot_index_ + 1, Slot{.i = 0});
            break;
          case classfile::ConstantTag::kDouble:
            static_inits.emplace_back(
              field.slot_index_,
              Slot{.d = static_cast<const classfile::DoubleInfo*>(cp_info)->value});
            static_inits.emplace_back(field.slot_index_ + 1, Slot{.i = 0});
            break;
          case classfile::ConstantTag::kString:
            // TODO: string intern
            break;
          default:
            throw std::runtime_error("invalid cp tag in preparing statics");
        }
      }
    } else {
      field.slot_index_ = instance_slot_count;
      instance_slot_count += (descriptor == "J" || descriptor == "D") ? 2 : 1;
    }
    fields_.push_back(field);
  }
  instance_slot_count_ = instance_slot_count;
  static_slot_count_   = static_slot_count;
  statics_.resize(static_slot_count);
  for (auto& p : static_inits) {
    statics_[p.first] = p.second;
  }
}

void InstanceKlass::linkSuperClass(classfile::ClassFile* cf, classfile::ClassLoader* loader) {
  U2 super_class_index = cf->super_class_index;
  if (super_class_index == 0) {
    this->setSuperClass(nullptr);
    return;
  }
  const auto& cp               = cf->constant_pool;
  std::string super_class_name = cp.getClassName(super_class_index);
  if (super_class_name == "java/lang/Object") {
    this->setSuperClass(nullptr);
    return;
  }
  auto* super_klass = loader->loadClass(super_class_name);
  // loadClass returns Klass*; the super of an InstanceKlass is always an
  // InstanceKlass
  this->setSuperClass(static_cast<InstanceKlass*>(super_klass));
}

void InstanceKlass::linkInterfaces(classfile::ClassFile* cf, classfile::ClassLoader* loader) {
  auto        interfaces = cf->interfaces;
  const auto& cp         = cf->constant_pool;
  for (U2 i = 0; i < interfaces.size(); i++) {
    U2          interface_index = interfaces[i];
    std::string interface_name  = cp.getClassName(interface_index);
    auto*       interface_klass = loader->loadClass(interface_name);
    this->setInterface(i, static_cast<InstanceKlass*>(interface_klass));
  }
}

// ============================================================================
// TypeArrayKlass  isInstanceOf
// ============================================================================

bool TypeArrayKlass::isInstanceOf(const Klass* target) const {
  if (this == target) {
    return true;
  }
  // All arrays are instances of java.lang.Object
  if (target != nullptr && target->getName() == "java/lang/Object") {
    return true;
  }
  return false;
}

// ============================================================================
// ObjArrayKlass  isInstanceOf
// ============================================================================

bool ObjArrayKlass::isInstanceOf(const Klass* target) const {
  if (this == target) {
    return true;
  }
  if (target != nullptr && target->getName() == "java/lang/Object") {
    return true;
  }
  // Covariant array assignment: String[] is Object[]
  if (target != nullptr && target->kind() == Klass::Kind::ObjArray) {
    const auto* target_arr = static_cast<const ObjArrayKlass*>(target);
    // Element-level check (currently exact only, no recursive covariance)
    if (element_klass_ == target_arr->element_klass_) {
      return true;
    }
  }
  return false;
}

}  // namespace jvm::oops