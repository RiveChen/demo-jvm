#include "descriptor.h"

#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "utilities/types.h"

namespace jvm::descriptor {

namespace {
TypeKind nextFieldType(std::string_view d, size_t& i) {
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
}  // namespace

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
  auto ret = (desc[i] == 'V') ? TypeKind::Void : nextFieldType(desc, i);
  return {.params = params, .ret = ret};
}

jvm::U2 argSlotCount(const MethodType& methodType) {
  jvm::U2 res = 0;
  for (const auto& param : methodType.params) {
    res += slotCount(param);
  }
  return res;
}

};  // namespace jvm::descriptor