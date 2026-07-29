/**
 * @file class_loader.cpp
 * @brief Class file reading, parsing orchestration, and Klass definition.
 */

#include "class_loader.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <span>
#include <vector>

#include "class_file_parser.hpp"
#include "oops/klass.hpp"
#include "oops/method_area.hpp"
#include "utilities/logger.hpp"

namespace jvm::classfile {

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
      LOG_DEBUG("Found class file: ", full_path.string(), " (", size, " bytes)");
      if (file.read(std::bit_cast<char*>(buffer.data()), size)) {
        return buffer;
      }
    }
  }
  return std::nullopt;
}

// (documentation in class_loader.h)
// NOLINTNEXTLINE(misc-no-recursion)
oops::Klass* ClassLoader::loadClass(const std::string& fully_qualified_name) {
  LOG_DEBUG("Loading class: ", fully_qualified_name);
  // Normalize to the internal (slash) form; callers may pass '.' or '/'.
  // From here on, slash is the single canonical class-name form (cache key,
  // MethodArea key, Klass::name_, path all agree).
  std::string name = fully_qualified_name;
  std::ranges::replace(name, '.', '/');

  // Check cache first to avoid re-loading already loaded classes
  if (cache_.contains(name)) {
    return cache_[name];
  }

  // Read class file from classpath
  auto class_file_data = readClassFile(name);
  if (!class_file_data) {
    throw std::runtime_error("Class " + name + " not found");
  }

  // Parse the class file and create Klass object
  auto  parser = ClassFileParser(std::span<U1>(std::bit_cast<U1*>(class_file_data.value().data()),
                                               class_file_data.value().size()));
  auto  class_file = parser.parse();
  auto* cf = class_file.get();  // grab before the move (pointee stays put; MethodArea owns it)

  // Create new Klass object from the parsed class file
  auto         klass     = std::make_unique<oops::Klass>(class_file.get(), this);
  oops::Klass* klass_ptr = klass.get();

  // Register the class in the method area with this class loader
  oops::MethodArea::getSingleton().addClass(
    std::make_pair(this, name), std::make_pair(std::move(klass), std::move(class_file)));

  // Cache the loaded class for future access
  cache_[name] = klass_ptr;

  klass_ptr->markLoaded();

  // Link the class
  klass_ptr->link(cf, this);
  LOG_INFO("Class loaded: ", name);

  return klass_ptr;
}

}  // namespace jvm::classfile