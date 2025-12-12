#include "constant_pool.h"

#include <bit>

#include "byte_reader.hpp"
#include "common/types.h"

using namespace common;

namespace class_loader {

// Utf8Info::readInfo() - reads a UTF-8 string
void Utf8Info::readInfo(ByteReader& reader) {
  U2 length = reader.read<U2>();
  value.resize(length);
  // TODO: implement Modified UTF-8 decoding
  reader.readBytes(value.data(), length);
}

// IntegerInfo::readInfo() - reads a 32-bit signed integer
void IntegerInfo::readInfo(ByteReader& reader) {
  U4 bits = reader.read<U4>();
  value   = std::bit_cast<Jint>(bits);
}

// FloatInfo::readInfo() - reads a 32-bit IEEE 754 float
void FloatInfo::readInfo(ByteReader& reader) {
  U4 bits = reader.read<U4>();
  value   = std::bit_cast<Jfloat>(bits);
}

// LongInfo::readInfo() - reads a 64-bit signed integer
void LongInfo::readInfo(ByteReader& reader) {
  U8 bits = reader.read<U8>();
  value   = std::bit_cast<Jlong>(bits);
}

// DoubleInfo::readInfo() - reads a 64-bit IEEE 754 double
void DoubleInfo::readInfo(ByteReader& reader) {
  U8 bits = reader.read<U8>();
  value   = std::bit_cast<Jdouble>(bits);
}

// ClassInfo::readInfo() - reads a class reference
void ClassInfo::readInfo(ByteReader& reader) { name_index = reader.read<U2>(); }

// StringInfo::readInfo() - reads a string reference
void StringInfo::readInfo(ByteReader& reader) { string_index = reader.read<U2>(); }

// MemberrefInfo::readInfo() - reads a member reference (field/method/interface
// method)
void MemberrefInfo::readInfo(ByteReader& reader) {
  class_index         = reader.read<U2>();
  name_and_type_index = reader.read<U2>();
}

// NameAndTypeInfo::readInfo() - reads a name and type descriptor
void NameAndTypeInfo::readInfo(ByteReader& reader) {
  name_index       = reader.read<U2>();
  descriptor_index = reader.read<U2>();
}

// MethodHandleInfo::readInfo() - reads a method handle reference
void MethodHandleInfo::readInfo(ByteReader& reader) {
  reference_kind  = reader.read<U1>();
  reference_index = reader.read<U2>();
}

// MethodTypeInfo::readInfo() - reads a method type descriptor
void MethodTypeInfo::readInfo(ByteReader& reader) { descriptor_index = reader.read<U2>(); }

// InvokeDynamicInfo::readInfo() - reads an invoke dynamic reference
void InvokeDynamicInfo::readInfo(ByteReader& reader) {
  bootstrap_method_attr_index = reader.read<U2>();
  name_and_type_index         = reader.read<U2>();
}

// ConstantPool methods
std::string ConstantPool::getUtf8String(U2 index) const {
  if (index == 0 || index >= pool_.size()) {
    throw std::runtime_error("Invalid constant pool index: " + std::to_string(index));
  }

  const auto* utf8_info = dynamic_cast<const Utf8Info*>(getConstantInfo(index));
  if (!utf8_info) {
    throw std::runtime_error("Constant pool entry at index " + std::to_string(index) +
                             " is not a UTF-8 string");
  }

  return utf8_info->value;
}

std::string ConstantPool::getClassName(U2 class_index) const {
  if (class_index == 0 || class_index >= pool_.size()) {
    throw std::runtime_error("Invalid constant pool index: " + std::to_string(class_index));
  }

  const auto* class_info = dynamic_cast<const ClassInfo*>(getConstantInfo(class_index));
  if (!class_info) {
    throw std::runtime_error("Constant pool entry at index " + std::to_string(class_index) +
                             " is not a class reference");
  }

  return getUtf8String(class_info->name_index);
}

std::pair<std::string, std::string> ConstantPool::getNameAndType(U2 index) const {
  if (index == 0 || index >= pool_.size()) {
    throw std::runtime_error("Invalid constant pool index: " + std::to_string(index));
  }

  const auto* name_and_type_info = dynamic_cast<const NameAndTypeInfo*>(getConstantInfo(index));
  if (!name_and_type_info) {
    throw std::runtime_error("Constant pool entry at index " + std::to_string(index) +
                             " is not a name and type");
  }

  std::string name       = getUtf8String(name_and_type_info->name_index);
  std::string descriptor = getUtf8String(name_and_type_info->descriptor_index);

  return std::make_pair(name, descriptor);
}

const ConstantInfo* ConstantPool::getConstantInfo(U2 index) const {
  if (index == 0 || index >= pool_.size()) {
    throw std::invalid_argument("Invalid constant pool index: " + std::to_string(index));
  }

  return pool_[index].get();
}

}  // namespace class_loader