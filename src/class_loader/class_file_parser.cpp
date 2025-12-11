#include "class_file_parser.h"

using namespace common;

namespace class_loader {

std::unique_ptr<ClassFile> ClassFileParser::parse() {
  parseMagic();
  auto version = parseVersion();

  return std::make_unique<ClassFile>(version);
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

}  // namespace class_loader