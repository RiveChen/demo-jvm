/**
 * @file descriptor.hpp
 * @brief Parsers for JVM field and method descriptors.
 *
 * Converts JVM descriptor strings (as defined in JVM Spec §4.3)
 * into typed representations for runtime method invocation.
 *
 * Example: `parseMethodDescriptor("(ID)V")` produces
 * `MethodType{params={Int, Double}, ret=Void, arg_slot_count=3}`.
 */

#pragma once

#include "utilities/types.hpp"

#include <cstdint>
#include <string_view>
#include <vector>

namespace jvm::descriptor {

/// @brief Enumeration of JVM type kinds recognized in descriptors.
enum class TypeKind : std::uint8_t {
  Int,     ///< I: int, byte, short, char, boolean (category 1)
  Long,    ///< J: long (category 2)
  Float,   ///< F: float (category 1)
  Double,  ///< D: double (category 2)
  Ref,     ///< L\<ClassName\>; or [\<type\>: reference/array (category 1)
  Void,    ///< V: void descriptor (return type only)
};

/// @brief Return the number of stack slots occupied by the given type.
/// @param type A TypeKind value.
/// @return 0 for Void, 2 for Long/Double, 1 otherwise.
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

/// @brief Check if a type is category 2 (occupies 2 stack slots).
/// @param type A TypeKind value.
/// @return true for Long and Double, false otherwise.
inline bool isCategory2(TypeKind type) {
  return type == TypeKind::Long || type == TypeKind::Double;
}

/// @brief Parse a single field type descriptor character.
/// @param desc The descriptor string view, e.g. "I" or "Ljava/lang/String;".
/// @return The corresponding TypeKind.
TypeKind parseFieldType(std::string_view desc);

/// @brief Parsed representation of a method descriptor.
///
/// Stores the parameter types, return type, and total argument slot count
/// (accounting for category-2 types occupying 2 slots each).
struct MethodType {
  std::vector<TypeKind> params;          ///< Parameter types in declaration order.
  TypeKind              ret;             ///< Return type.
  U2                    arg_slot_count;  ///< Total slot count across all parameters.

  /// Compares two MethodType values for equality (defaulted).
  bool operator==(const MethodType& other) const = default;
};

/// @brief Parse a method descriptor string into its structured representation.
/// @param desc A JVM method descriptor, e.g. "(IJ)V" or
/// "(Ljava/lang/String;)Z".
/// @return The parsed MethodType containing params, ret, and arg_slot_count.
MethodType parseMethodDescriptor(std::string_view desc);

}  // namespace jvm::descriptor