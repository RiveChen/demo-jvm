#include "members.h"

#include "class_file_parser.h"

namespace class_loader {

using namespace common;

void FieldInfo::readInfo(ClassFileParser& parser) {
  access_flags     = AccessFlags<flags::Field>(parser.getReader().read<U2>());
  name_index       = parser.getReader().read<U2>();
  descriptor_index = parser.getReader().read<U2>();
  attributes       = parser.parseAttributes();

  name = parser.getConstantPool().getUtf8String(name_index);
}

void MethodInfo::readInfo(ClassFileParser& parser) {
  access_flags     = AccessFlags<flags::Method>(parser.getReader().read<U2>());
  name_index       = parser.getReader().read<U2>();
  descriptor_index = parser.getReader().read<U2>();
  attributes       = parser.parseAttributes();

  name = parser.getConstantPool().getUtf8String(name_index);
}

}  // namespace class_loader
