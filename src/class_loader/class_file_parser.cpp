#include "class_file_parser.h"

using namespace common;

namespace class_loader {

std::unique_ptr<ClassFile> ClassFileParser::parse() {
  parseMagic();
  return std::make_unique<ClassFile>();
}

void ClassFileParser::parseMagic() {
  U4 magic = reader_.read<U4>();
  if (magic != kClassFileMagic) {
    throw std::runtime_error("Invalid class file magic number");
  }
}

}  // namespace class_loader