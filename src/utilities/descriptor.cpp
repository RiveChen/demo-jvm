/**
 * @file descriptor.cpp
 * @brief JVM descriptor string parser implementations.
 */

#include "descriptor.hpp"

#include "utilities/types.hpp"

#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace jvm::descriptor {

static TypeKind nextFieldType(std::string_view d,
                              size_t&          i) {  // NOLINT(misc-no-recursion)
  assert(i < d.size());
  char c = d[i];
  switch (c) {
    case 'B':
    case 'C':
    case 'I':
    case 'S':
    case 'Z':
      i++;
      return TypeKind::Int;
    case 'F':
      i++;
      return TypeKind::Float;
    case 'J':
      i++;
      return TypeKind::Long;
    case 'D':
      i++;
      return TypeKind::Double;
    case 'L':
      while (i < d.size() && d[i] != ';') {
        i++;
      }
      if (i >= d.size()) {
        throw std::runtime_error("invalid descriptor");
      }
      i++;
      return TypeKind::Ref;
    case '[':
      while (i < d.size() && d[i] == '[') {
        i++;
      }
      if (i >= d.size()) {
        throw std::runtime_error("invalid descriptor");
      }
      nextFieldType(d, i);
      return TypeKind::Ref;
    default:
      throw std::runtime_error("invalid descriptor");
  }
}

TypeKind parseFieldType(std::string_view desc) {
  size_t i    = 0;
  auto   type = nextFieldType(desc, i);
  assert(i == desc.size());
  return type;
}

MethodType parseMethodDescriptor(std::string_view desc) {
  assert(desc[0] == '(');
  size_t                i = 1;  // skip starting '('
  std::vector<TypeKind> params;
  while (i < desc.size() && desc[i] != ')') {
    params.push_back(nextFieldType(desc, i));
  }
  if (i >= desc.size()) {
    throw std::runtime_error("invalid descriptor");
  }
  i++;
  auto    ret            = (desc[i] == 'V') ? TypeKind::Void : nextFieldType(desc, i);
  jvm::U2 arg_slot_count = 0;
  for (const auto& param : params) {
    arg_slot_count += slotCount(param);
  }
  return {.params = params, .ret = ret, .arg_slot_count = arg_slot_count};
}

};  // namespace jvm::descriptor