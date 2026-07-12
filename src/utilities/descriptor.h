#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include "utilities/types.h"

namespace jvm::descriptor {

enum class TypeKind : std::uint8_t { Int, Long, Float, Double, Ref, Void };

inline int slotCount(TypeKind type) {
  switch (type) {
    case TypeKind::Void:
      return 0;
    case TypeKind::Long:
    case TypeKind::Double:
      return 2;
    case TypeKind::Int:
    case TypeKind::Float:
    case TypeKind::Ref:
      return 1;
  }
}

inline bool isCategory2(TypeKind type) {
  return type == TypeKind::Long || type == TypeKind::Double;
}

TypeKind parseFieldType(std::string_view desc);

struct MethodType {
  std::vector<TypeKind> params;
  TypeKind              ret;

  bool operator==(const MethodType& other) const = default;
};

MethodType parseMethodDescriptor(std::string_view desc);

U2 argSlotCount(const MethodType&);

}  // namespace jvm::descriptor
