/**
 * @file constant_pool.h
 * @brief Runtime constant pool with symbolic and resolved references.
 *
 * After a class is loaded, the class file constant pool is converted
 * into a RuntimeConstantPool (RtCp). Entries start as symbolic
 * references (SymRef_*) and are lazily resolved into concrete
 * Klass* Method* Field* pointers when first accessed.
 */

#pragma once

#include <string>
#include <variant>
#include <vector>

#include "utilities/types.h"

namespace jvm::oops {

/// @name Symbolic Reference Types (unresolved constant pool entries)
/// These are placeholders used before lazy resolution occurs.
///@{

/// @brief A symbolic reference to a class (unresolved).
struct SymRef_Class {
  std::string class_name;  ///< Fully qualified class name (e.g. "java.lang.Object")
};

/// @brief A symbolic reference to a field (unresolved).
struct SymRef_Field {
  U2          class_cp_index;  ///< Index into the owning class's constant pool for the class ref.
  std::string member_name;     ///< Field name.
  std::string descriptor;      ///< Field descriptor string.
};

/// @brief A symbolic reference to a method (unresolved).
struct SymRef_Method {
  U2          class_cp_index;  ///< Index into the owning class's constant pool for the class ref.
  std::string member_name;     ///< Method name.
  std::string descriptor;      ///< Method descriptor string.
};

/// @brief A symbolic reference to an interface method (unresolved).
struct SymRef_InterfaceMethod {
  U2          class_cp_index;  ///< Index into the owning class's constant pool for the class ref.
  std::string member_name;     ///< Interface method name.
  std::string descriptor;      ///< Interface method descriptor string.
};
///@}

class Klass;
class Method;
class Field;

/// @brief Variant type for runtime constant pool entries.
///
/// Can hold:
/// - std::monostate (empty/uninitialized slot)
/// - Literal values: Jint, Jfloat, Jlong, Jdouble, std::string
/// - Resolved references: Klass*, Method*, Field*
/// - Unresolved symbolic references: SymRef_Class, SymRef_Field, SymRef_Method,
/// SymRef_InterfaceMethod
using RtCpInfo =
  std::variant<std::monostate, Jint, Jfloat, Jlong, Jdouble, std::string, Klass*, Method*, Field*,
               SymRef_Class, SymRef_Field, SymRef_Method, SymRef_InterfaceMethod>;

/**
 * @brief The runtime constant pool for a class.
 *
 * Created by Klass::prepareRuntimeConstantPool() during class loading.
 * Contains all constants and symbolic references from the class file's
 * constant pool. Symbolic references are lazily resolved on first use.
 *
 * @see JVM Specification §4.11 "The Constant Pool"
 */
class RuntimeConstantPool {
 public:
  explicit RuntimeConstantPool(Klass* owner_klass) : owner_klass_(owner_klass) {}

  /// @brief Set an entry at the given constant pool index.
  void setConstant(U2 index, RtCpInfo info) { infos_.at(index) = std::move(info); }

  /// @brief Get the entry at the given index, without resolving.
  RtCpInfo getConstant(U2 index) { return infos_.at(index); }

  /// @name Lazy Resolution
  /// Resolve a symbolic reference into a concrete runtime pointer.
  ///@{
  Klass*  resolveClass(U2 index);
  Field*  resolveField(U2 index);
  Method* resolveMethod(U2 index);
  ///@}

  /// @brief Resolve the symbolic key string for an entry (e.g. for error messages).
  std::optional<std::string> symbolicKey(U2 index);

 private:
  std::vector<RtCpInfo> infos_;        ///< The constant pool entries.
  Klass*                owner_klass_;  ///< The class that owns this constant pool.

  friend class Klass;
};

}  // namespace jvm::oops