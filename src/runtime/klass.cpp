#include "klass.h"

#include "class_loader/class_file.h"
#include "runtime/constant_pool.h"

namespace jvm::runtime {

Klass::Klass(class_loader::ClassFile* class_file, class_loader::ClassLoader* loader)
  : loader_(loader),
    class_file_(class_file),
    name_(class_file->constant_pool.getClassName(class_file->this_class_index)),
    access_flags_(class_file->access_flags),
    super_class_(nullptr),
    interfaces_(class_file->interfaces_count),
    constant_pool_(this),
    mirror_class_object_(nullptr) {}

// NOLINTNEXTLINE(misc-no-recursion)
Method* Klass::findMethod(const std::string& name, const std::string& descriptor) {
  for (auto& method : methods_) {
    if (method.getName() == name && method.getDescriptor() == descriptor) {
      return &method;
    }
  }
  if (super_class_ != nullptr) {
    return super_class_->findMethod(name, descriptor);
  }
  return nullptr;
}

// NOLINTNEXTLINE(misc-no-recursion)
Field* Klass::findField(const std::string& name, const std::string& descriptor) {
  for (auto& field : fields_) {
    if (field.getName() == name && field.getDescriptor() == descriptor) {
      return &field;
    }
  }
  if (super_class_ != nullptr) {
    return super_class_->findField(name, descriptor);
  }
  return nullptr;
}

void Klass::prepareRuntimeConstantPool(class_loader::ClassFile* class_file) {
  // create runtime constant pool
  size_t cp_count = class_file->constant_pool.size();
  constant_pool_.infos_.resize(cp_count);
  constant_pool_.infos_[0] = std::monostate{};  // placeholder for the first slot
  for (size_t i = 1; i < cp_count; i++) {
    const auto* cpinfo = class_file->constant_pool.getConstantInfo(i);
    switch (cpinfo->tag) {
      case class_loader::ConstantTag::kClass: {
        const auto* info = dynamic_cast<const class_loader::ClassInfo*>(cpinfo);
        constant_pool_.setConstant(i, SymRef_Class{.name_index = info->name_index});
      } break;
      case class_loader::ConstantTag::kMethodref: {
        const auto* info = dynamic_cast<const class_loader::MethodrefInfo*>(cpinfo);
        constant_pool_.setConstant(i,
                                   SymRef_Method{.class_index         = info->class_index,
                                                 .name_and_type_index = info->name_and_type_index});
      } break;
      case class_loader::ConstantTag::kFieldref: {
        const auto* info = dynamic_cast<const class_loader::FieldrefInfo*>(cpinfo);
        constant_pool_.setConstant(i,
                                   SymRef_Field{.class_index         = info->class_index,
                                                .name_and_type_index = info->name_and_type_index});
      } break;
      case class_loader::ConstantTag::kInterfaceMethodref: {
        const auto* info = dynamic_cast<const class_loader::InterfaceMethodrefInfo*>(cpinfo);
        constant_pool_.setConstant(
          i, SymRef_InterfaceMethod{.class_index         = info->class_index,
                                    .name_and_type_index = info->name_and_type_index});
      } break;

      case class_loader::ConstantTag::kInteger: {
        auto rt_cpinfo = dynamic_cast<const class_loader::IntegerInfo*>(cpinfo)->value;
        constant_pool_.setConstant(i, rt_cpinfo);
      } break;

      case class_loader::ConstantTag::kLong: {
        auto rt_cpinfo = dynamic_cast<const class_loader::LongInfo*>(cpinfo)->value;
        constant_pool_.setConstant(i, rt_cpinfo);
        i++;
        constant_pool_.infos_[i] = std::monostate{};  // placeholder for the second slot
      } break;
      case class_loader::ConstantTag::kFloat: {
        auto rt_cpinfo = dynamic_cast<const class_loader::FloatInfo*>(cpinfo)->value;
        constant_pool_.setConstant(i, rt_cpinfo);
      } break;
      case class_loader::ConstantTag::kDouble: {
        auto rt_cpinfo = dynamic_cast<const class_loader::DoubleInfo*>(cpinfo)->value;
        constant_pool_.setConstant(i, rt_cpinfo);
        i++;
        constant_pool_.infos_[i] = std::monostate{};  // placeholder for the second slot
      } break;

      case class_loader::ConstantTag::kString: {
        // TODO: string intern
        auto index     = dynamic_cast<const class_loader::StringInfo*>(cpinfo)->string_index;
        auto rt_cpinfo = class_file->constant_pool.getUtf8String(index);
        constant_pool_.setConstant(i, rt_cpinfo);
      } break;

      // TODO: dynamic language support
      case class_loader::ConstantTag::kMethodHandle:
        break;
      case class_loader::ConstantTag::kMethodType:
        break;
      case class_loader::ConstantTag::kInvokeDynamic:
        break;

      // just ignore these constant pool entries
      case class_loader::ConstantTag::kNameAndType:
      case class_loader::ConstantTag::kUtf8:
        break;
      default:
        throw std::runtime_error("Unknown constant pool tag: " +
                                 std::to_string(static_cast<int>(cpinfo->tag)));
    }
  }
}

void Klass::prepareMethods(class_loader::ClassFile* class_file) {
  // create methods
  for (auto& member_info : class_file->methods.getMembers()) {
    auto* method_info  = dynamic_cast<class_loader::MethodInfo*>(member_info.get());
    auto  access_flags = method_info->access_flags;
    // optimize after string intern
    auto   name       = class_file->constant_pool.getUtf8String(method_info->name_index);
    auto   descriptor = class_file->constant_pool.getUtf8String(method_info->descriptor_index);
    Method method(access_flags, name, descriptor, this);
    if (access_flags.has(flags::Method::NATIVE)) {
      // TODO: native method binding
      // this->linkNativeMethods(&method);
    } else if (!access_flags.has(flags::Method::ABSTRACT)) {
      // if a method is not native and not abstract, it must have code
      // find Code attribute
      auto* code_attribute = method_info->attributes.getAttribute<class_loader::CodeAttribute>();
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

void Klass::prepareFieldsAndStatics(class_loader::ClassFile* class_file) {
  // create fields
  size_t instance_slot_count = 0;
  size_t static_slot_count   = 0;
  for (auto& member_info : class_file->fields.getMembers()) {
    auto* field_info   = dynamic_cast<class_loader::FieldInfo*>(member_info.get());
    auto  access_flags = field_info->access_flags;
    // optimize after string intern
    auto  name       = class_file->constant_pool.getUtf8String(field_info->name_index);
    auto  descriptor = class_file->constant_pool.getUtf8String(field_info->descriptor_index);
    Field field(access_flags, name, descriptor, this);
    if (access_flags.has(flags::Field::STATIC)) {
      field.slot_index_ = static_slot_count;
      static_slot_count += (descriptor == "J" || descriptor == "D") ? 2 : 1;
    } else {
      field.slot_index_ = instance_slot_count;
      instance_slot_count += (descriptor == "J" || descriptor == "D") ? 2 : 1;
    }
    fields_.push_back(field);
  }
  instance_slot_count_ = instance_slot_count;
  static_slot_count_   = static_slot_count;
}

// void Klass::linkNativeMethods(runtime::Method* method) {
//   std::string key        = this->name_ + "::" + method->getName() + ":" +
//   method->getDescriptor(); auto        native_ptr =
//   native::NativeRegistry::getInstance().findNativeMethod(key); if (native_ptr) {
//     method->native_function = native_ptr;
//   }
// }

}  // namespace jvm::runtime