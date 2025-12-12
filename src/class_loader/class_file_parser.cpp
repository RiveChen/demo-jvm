#include "class_file_parser.h"

using namespace common;

namespace class_loader {

std::unique_ptr<ClassFile> ClassFileParser::parse() {
  parseMagic();
  auto version           = parseVersion();
  auto constant_pool     = parseConstantPool();
  auto access_flags      = parseAccessFlags();
  auto this_class_index  = reader_.read<U2>();
  auto super_class_index = reader_.read<U2>();
  auto interfaces_count  = reader_.read<U2>();
  auto interfaces        = std::vector<common::U2>(interfaces_count);
  for (common::U2 i = 0; i < interfaces_count; ++i) {
    interfaces[i] = reader_.read<U2>();
  }
  return std::make_unique<ClassFile>(std::move(version), std::move(constant_pool),
                                     std::move(access_flags), this_class_index, super_class_index,
                                     interfaces_count, std::move(interfaces));
}

void ClassFileParser::parseMagic() {
  U4 magic = reader_.read<U4>();
  if (magic != kClassFileMagic) {
    throw std::runtime_error("Invalid class file magic number");
  }
}

Version ClassFileParser::parseVersion() {
  // the minor version is read first, then the major version
  U2 minor = reader_.read<U2>();
  U2 major = reader_.read<U2>();
  return Version(major, minor);
}

ConstantPool ClassFileParser::parseConstantPool() {
  // The value of the constant_pool_count is equal to the number of entries in
  // the constant_pool table plus one
  U2                                         count = reader_.read<U2>();
  std::vector<std::unique_ptr<ConstantInfo>> pool;
  // The constant pool is 1-indexed, so a count of n means indices 1 to n-1.
  // We create a vector with a null placeholder at index 0 for convenience.
  pool.resize(count);

  for (common::U2 i = 1; i < count; ++i) {
    pool[i] = createConstantInfo();

    // Special case: Long and Double constants take up two slots in the pool.
    // See JVM Spec §4.4.5
    auto tag = pool[i]->tag;
    if (tag == ConstantTag::kLong || tag == ConstantTag::kDouble) {
      i++;  // Skip the next entry
    }
  }

  return ConstantPool(std::move(pool));
}

// The factory method
std::unique_ptr<ConstantInfo> ClassFileParser::createConstantInfo() {
  auto tag_byte = reader_.read<U1>();
  auto tag      = static_cast<ConstantTag>(tag_byte);

  std::unique_ptr<ConstantInfo> info;
  switch (tag) {
    case ConstantTag::kUtf8:
      info = std::make_unique<Utf8Info>();
      break;
    case ConstantTag::kClass:
      info = std::make_unique<ClassInfo>();
      break;
    case ConstantTag::kString:
      info = std::make_unique<StringInfo>();
      break;
    case ConstantTag::kFieldref:
      info = std::make_unique<FieldrefInfo>();
      break;
    case ConstantTag::kMethodref:
      info = std::make_unique<MethodrefInfo>();
      break;
    case ConstantTag::kInterfaceMethodref:
      info = std::make_unique<InterfaceMethodrefInfo>();
      break;
    case ConstantTag::kNameAndType:
      info = std::make_unique<NameAndTypeInfo>();
      break;
    case ConstantTag::kInteger:
      info = std::make_unique<IntegerInfo>();
      break;
    case ConstantTag::kLong:
      info = std::make_unique<LongInfo>();
      break;
    case ConstantTag::kFloat:
      info = std::make_unique<FloatInfo>();
      break;
    case ConstantTag::kDouble:
      info = std::make_unique<DoubleInfo>();
      break;
    case ConstantTag::kMethodHandle:
      info = std::make_unique<MethodHandleInfo>();
      break;
    case ConstantTag::kMethodType:
      info = std::make_unique<MethodTypeInfo>();
      break;
    case ConstantTag::kInvokeDynamic:
      info = std::make_unique<InvokeDynamicInfo>();
      break;
    default:
      throw std::runtime_error("Unknown constant pool tag: " + std::to_string(tag_byte));
  }

  info->tag = tag;
  info->readInfo(reader_);  // Delegate parsing of specific data to the object itself

  return info;
}

AccessFlags<flags::Class> ClassFileParser::parseAccessFlags() {
  U2 flags = reader_.read<U2>();
  return AccessFlags<flags::Class>(flags);
}

}  // namespace class_loader