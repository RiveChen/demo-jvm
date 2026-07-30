/**
 * @file method_area.h
 * @brief The method area (class metadata storage).
 *
 * Implements the Method Area as described in JVM Specification §2.5.4.
 * This is a singleton that stores all loaded classes (Klass objects)
 * and their associated parsed ClassFile data, keyed by
 * (ClassLoader*, className).
 *
 * @note Currently single-threaded with no cleanup mechanism.
 *       MethodArea::reset() exists for testing but there is no
 *       invalidation notification to ClassLoader caches.
 */

#pragma once

#include "classfile/class_file.hpp"
#include "classfile/class_loader.hpp"
#include "klass.hpp"
#include "utilities/basic_type.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace jvm::oops {

/// @brief Hash functor for (ClassLoader*, className) pairs.
struct ClassIdentifierHash {
  std::size_t operator()(const std::pair<classfile::ClassLoader*, std::string>& p) const {
    return std::hash<void*>{}(p.first) ^ (std::hash<std::string>{}(p.second) << 1U);
  }
};

/**
 * @brief Singleton storing all loaded classes.
 *
 * Each class is identified by its defining ClassLoader and fully-qualified
 * name. The Klass and original ClassFile are stored together as a pair.
 */
class MethodArea {
 public:
  using ClassIdentifier = std::pair<classfile::ClassLoader*, std::string>;
  using ClassData       = std::pair<std::unique_ptr<InstanceKlass>, std::unique_ptr<classfile::ClassFile>>;

  /// Meyer's singleton (thread-safe since C++11).
  static MethodArea& getSingleton() {
    static MethodArea singleton;
    return singleton;
  }

  /// @brief Add a class to the method area.
  void addClass(ClassIdentifier identifier, ClassData class_data);

  /// @brief Look up a class by (ClassLoader, name).
  /// @return nullptr if the class is not found.
  InstanceKlass* getClass(const ClassIdentifier& identifier);

  /// @brief Check if a class has been loaded.
  bool hasClass(const ClassIdentifier& identifier) const;

  /// @brief Clear all loaded classes (only for testing).
  void reset() {
    instance_klasses_.clear();
    array_klasses_.clear();
  }

  TypeArrayKlass* getOrCreateTypeArrayKlass(BasicType type);
  ObjArrayKlass*  getOrCreateObjArrayKlass(Klass* component);

  MethodArea(const MethodArea&)            = delete;
  MethodArea(MethodArea&&)                 = delete;
  MethodArea& operator=(const MethodArea&) = delete;
  MethodArea& operator=(MethodArea&&)      = delete;

 private:
  MethodArea()  = default;
  ~MethodArea() = default;

  std::unordered_map<ClassIdentifier, ClassData, ClassIdentifierHash> instance_klasses_;
  std::unordered_map<std::string, std::unique_ptr<ArrayKlass>>        array_klasses_;
};

}  // namespace jvm::oops