#include "class_loader.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <span>
#include <vector>

#include "class_file_parser.h"
#include "runtime/klass.h"
#include "runtime/method_area.h"

namespace jvm::class_loader {

/**
 * @brief Reads a class file from the classpath and returns its binary content
 *
 * This method searches for a class file by converting the class name to a file path
 * (replacing dots with forward slashes and appending .class extension) and looking
 * through all configured classpath directories.
 *
 * @param name The fully qualified class name (e.g., "java.lang.String")
 * @return std::optional<std::vector<std::byte>> The binary content of the class file if found,
 *         std::nullopt if the class file could not be located or read
 *
 * @note The method searches through classpaths in order and returns the first match found
 * @note Class names are converted to file paths by replacing '.' with '/' and adding '.class'
 */
std::optional<std::vector<std::byte>> ClassLoader::readClassFile(const std::string& name) {
  // Convert class name to file path (e.g., "java.lang.String" -> "java/lang/String.class")
  std::string relative_path = name;
  std::ranges::replace(relative_path, '.', '/');
  relative_path.append(".class");

  // Search through all configured classpath directories
  for (const auto& classpath : classpaths_) {
    std::filesystem::path full_path = std::filesystem::path(classpath) / relative_path;
    if (std::filesystem::exists(full_path) && std::filesystem::is_regular_file(full_path)) {
      // Open file in binary mode and get file size
      std::ifstream file(full_path, std::ios::binary | std::ios::ate);
      if (!file) {
        continue;
      }
      std::streamsize size = file.tellg();
      if (size < 0) {
        continue;
      }
      file.seekg(0, std::ios::beg);
      std::vector<std::byte> buffer(static_cast<size_t>(size));
      if (file.read(std::bit_cast<char*>(buffer.data()), size)) {
        return buffer;
      }
    }
  }
  return std::nullopt;
}

/**
 * @brief Defines a new class from a parsed ClassFile and registers it in the method area
 *
 * This method creates a new Klass object from the parsed class file data and registers
 * it in the method area with the current class loader. The class becomes available
 * for use by the JVM runtime.
 *
 * @param class_file A unique pointer to the parsed ClassFile containing class metadata
 * @param name The fully qualified name of the class being defined
 * @return runtime::Klass* A pointer to the newly created and registered Klass object
 *
 * @note The method takes ownership of the ClassFile and transfers it to the method area
 * @note The created Klass object is associated with this ClassLoader instance
 * @note The class is registered in the method area using a key of (ClassLoader*, name)
 */
// NOLINTNEXTLINE(misc-no-recursion)
runtime::Klass* ClassLoader::defineClass(std::unique_ptr<class_loader::ClassFile> class_file,
                                         const std::string&                       name) {
  // Create new Klass object from the parsed class file
  auto            klass     = std::make_unique<runtime::Klass>(class_file.get(), this);
  runtime::Klass* klass_ptr = klass.get();

  linkSuperClass(klass_ptr);
  linkInterfaces(klass_ptr);

  // Register the class in the method area with this class loader
  runtime::MethodArea::getInstance().addClass(
    std::make_pair(this, name), std::make_pair(std::move(klass), std::move(class_file)));
  return klass_ptr;
}

// NOLINTNEXTLINE(misc-no-recursion)
void ClassLoader::linkSuperClass(runtime::Klass* klass) {
  U2 super_class_index = klass->getClassFile()->super_class_index;

  if (super_class_index == 0) {
    // only `java.lang.Object` has no super class
    klass->setSuperClass(nullptr);
    return;
  }

  const auto& cp               = klass->getClassFile()->constant_pool;
  std::string super_class_name = cp.getClassName(super_class_index);
  std::replace(super_class_name.begin(), super_class_name.end(), '/', '.');

  if (super_class_name == "java.lang.Object") {
    // suspend the support of Object for now
    klass->setSuperClass(nullptr);
    return;
  }

  auto* super_klass = this->loadClass(super_class_name);

  klass->setSuperClass(super_klass);
}

// NOLINTNEXTLINE(misc-no-recursion)
void ClassLoader::linkInterfaces(runtime::Klass* klass) {
  auto        interfaces = klass->getClassFile()->interfaces;
  const auto& cp         = klass->getClassFile()->constant_pool;
  for (auto& interface_index : interfaces) {
    std::string interface_name = cp.getClassName(interface_index);
    std::replace(interface_name.begin(), interface_name.end(), '/', '.');
    auto* interface_klass = loadClass(interface_name);
    klass->setInterface(interface_index, interface_klass);
  }
}

/**
 * @brief Loads a class by name, parsing it from the classpath and creating a Klass object
 *
 * This is the main entry point for class loading. It first checks the cache for an
 * already loaded class, and if not found, reads the class file from the classpath,
 * parses it, and creates a new Klass object. The loaded class is cached for future
 * access.
 *
 * @param fully_qualified_name The fully qualified class name to load (e.g., "java.lang.String")
 * @return runtime::Klass* A pointer to the loaded Klass object, or nullptr if the
 *         class could not be found or loaded
 *
 * @note This method implements caching - subsequent calls with the same name return
 *       the cached Klass object without re-parsing the class file
 * @note If the class file cannot be found, an error message is printed to stderr
 * @note The method handles the complete class loading process: file reading, parsing,
 *       and Klass object creation
 */
// NOLINTNEXTLINE(misc-no-recursion)
runtime::Klass* ClassLoader::loadClass(const std::string& fully_qualified_name) {
  // Check cache first to avoid re-loading already loaded classes
  if (cache_.find(fully_qualified_name) != cache_.end()) {
    return cache_[fully_qualified_name];
  }

  // Read class file from classpath
  auto class_file_data = readClassFile(fully_qualified_name);
  if (!class_file_data) {
    throw std::runtime_error("Class " + fully_qualified_name + " not found");
  }

  // Parse the class file and create Klass object
  auto parser = ClassFileParser(std::span<U1>(std::bit_cast<U1*>(class_file_data.value().data()),
                                              class_file_data.value().size()));
  auto class_file = parser.parse();

  auto* klass = defineClass(std::move(class_file), fully_qualified_name);

  // Prepare the class
  klass->prepareRuntimeConstantPool(klass->getClassFile());
  klass->prepareMethods(klass->getClassFile());
  klass->prepareFieldsAndStatics(klass->getClassFile());

  // Cache the loaded class for future access
  cache_[fully_qualified_name] = klass;
  return klass;
}

}  // namespace jvm::class_loader