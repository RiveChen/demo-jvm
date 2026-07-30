#pragma once

#include "types.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace jvm {
enum BasicType : uint8_t {
  T_BOOLEAN = 4,
  T_CHAR    = 5,
  T_FLOAT   = 6,
  T_DOUBLE  = 7,
  T_BYTE    = 8,
  T_SHORT   = 9,
  T_INT     = 10,
  T_LONG    = 11,
};

struct BasicTypeInfo {
  size_t      size;
  const char* arrayName;
  const char* typeName;
};

constexpr std::array<BasicTypeInfo, 12> kTypeInfo = {{
  {.size = 0, .arrayName = "", .typeName = ""},
  {.size = 0, .arrayName = "", .typeName = ""},
  {.size = 0, .arrayName = "", .typeName = ""},
  {.size = 0, .arrayName = "", .typeName = ""},
  {.size = 1, .arrayName = "[Z", .typeName = "boolean"},
  {.size = 2, .arrayName = "[C", .typeName = "char"},
  {.size = 4, .arrayName = "[F", .typeName = "float"},
  {.size = 8, .arrayName = "[D", .typeName = "double"},
  {.size = 1, .arrayName = "[B", .typeName = "byte"},
  {.size = 2, .arrayName = "[S", .typeName = "short"},
  {.size = 4, .arrayName = "[I", .typeName = "int"},
  {.size = 8, .arrayName = "[J", .typeName = "long"},
}};

inline Jint basicTypeElementSize(BasicType type) { return kTypeInfo[static_cast<size_t>(type)].size; }

inline std::string basicTypeToArrayName(BasicType type) { return kTypeInfo[static_cast<size_t>(type)].arrayName; }

inline BasicType charToBasicType(char c) {
  switch (c) {
    case 'Z':
      return T_BOOLEAN;
    case 'C':
      return T_CHAR;
    case 'F':
      return T_FLOAT;
    case 'D':
      return T_DOUBLE;
    case 'B':
      return T_BYTE;
    case 'S':
      return T_SHORT;
    case 'I':
      return T_INT;
    case 'J':
      return T_LONG;
    default:
      throw std::runtime_error("invalid array element type descriptor");
  }
}

inline BasicType atypeToBasicType(const std::string& atype) {
  static const std::unordered_map<std::string, BasicType> kMap = [] {
    std::unordered_map<std::string, BasicType> m;
    for (const auto& info : kTypeInfo) {
      if (info.typeName[0] != '\0') {
        m.emplace(info.typeName, static_cast<BasicType>(&info - kTypeInfo.data()));
      }
    }
    return m;
  }();
  auto it = kMap.find(atype);
  if (it != kMap.end()) {
    return it->second;
  }

  throw std::runtime_error("invalid atype");
}

}  // namespace jvm