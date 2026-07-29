/**
 * @file attributes.cpp
 * @brief Class file attribute parsing implementations.
 *
 * Implements readInfo() for all attribute types: GenericAttribute,
 * ConstantValueAttribute, ExceptionsAttribute, BootstrapMethodsAttribute,
 * CodeAttribute, and their nested structures.
 */

#include "attributes.hpp"

#include "class_file_parser.hpp"

namespace jvm::classfile {

void GenericAttribute::readInfo(ClassFileParser& parser) {
  // Read the raw bytes based on the attribute_length set by the factory
  info = parser.getReader().readBytes(length);
}

void ConstantValueAttribute::readInfo(ClassFileParser& parser) {
  value = parser.getReader().read<U2>();
}

void ExceptionsAttribute::readInfo(ClassFileParser& parser) {
  num_exceptions = parser.getReader().read<U2>();
  exceptions     = std::vector<U2>(num_exceptions);
  for (U2 i = 0; i < num_exceptions; ++i) {
    exceptions[i] = parser.getReader().read<U2>();
  }
}

void BootstrapMethodsAttribute::readInfo(ClassFileParser& parser) {
  num_bootstrap_methods = parser.getReader().read<U2>();
  bootstrap_methods.reserve(num_bootstrap_methods);

  for (U2 i = 0; i < num_bootstrap_methods; ++i) {
    BootstrapMethod method;
    method.method_ref = parser.getReader().read<U2>();

    method.num_arguments = parser.getReader().read<U2>();
    method.arguments.reserve(method.num_arguments);
    for (U2 j = 0; j < method.num_arguments; ++j) {
      method.arguments.push_back(parser.getReader().read<U2>());
    }
    bootstrap_methods.push_back(std::move(method));
  }
}

void CodeAttribute::readInfo(ClassFileParser& parser) {
  max_stack           = parser.getReader().read<U2>();
  max_locals          = parser.getReader().read<U2>();
  code_length         = parser.getReader().read<U4>();
  code                = parser.getReader().readBytes(code_length);
  num_exception_table = parser.getReader().read<U2>();
  exception_table     = std::vector<ExceptionTableEntry>(num_exception_table);
  for (U2 i = 0; i < num_exception_table; ++i) {
    exception_table[i] = {
      .start_pc   = parser.getReader().read<U2>(),
      .end_pc     = parser.getReader().read<U2>(),
      .handler_pc = parser.getReader().read<U2>(),
      .catch_type = parser.getReader().read<U2>(),
    };
  }
  attributes = parser.parseAttributes();
}

}  // namespace jvm::classfile
