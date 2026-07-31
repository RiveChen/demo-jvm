/**
 * @file class_loader.hpp
 * @brief Class loading subsystem.
 *
 * Implements class loading as described in JVM Specification §5.3.
 * The ClassLoader reads .class files from classpath directories,
 * delegates to the ClassFileParser, and creates Klass objects in
 * the MethodArea.
 *
 * @note Currently supports a parent-delegation model but does not
 *       implement the full bootstrap/ext/application loader chain.
 */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace jvm::oops {
class InstanceKlass;
}  // namespace jvm::oops

namespace jvm::classfile {

/**
 * @brief Loads Java class files and produces runtime Klass objects.
 *
 * For each class, the loader:
 * 1. Reads the .class file bytes from the classpath.
 * 2. Parses them via ClassFileParser into a ClassFile.
 * 3. Constructs and initializes a Klass object in the MethodArea.
 * 4. Links super class and interfaces recursively.
 *
 * @note cache_ stores raw pointers to Klass objects owned by MethodArea.
 *       If MethodArea::reset() is called, the cache becomes stale.
 */
class ClassLoader {
 private:
  ClassLoader*             parent_;      ///< Parent loader for delegation.
  std::vector<std::string> classpaths_;  ///< Directories to search for .class files.
  std::unordered_map<std::string, oops::InstanceKlass*>
    cache_;  ///< Loaded class cache (raw ptr, not owning).

  /// @brief Read raw class file bytes from the classpath.
  std::optional<std::vector<std::byte>> readClassFile(const std::string& name);

 public:
  explicit ClassLoader(ClassLoader* parent = nullptr, std::vector<std::string> classpaths = {})
    : parent_(parent), classpaths_(std::move(classpaths)) {}

  /// @brief Load (or find already-loaded) a class by fully qualified name.
  /// @param fully_qualified_name Fully qualified class name (e.g.
  /// "java.lang.Object").
  /// @return The Klass object, or nullptr if the class cannot be found.
  oops::InstanceKlass* loadClass(const std::string& fully_qualified_name);
};

}  // namespace jvm::classfile