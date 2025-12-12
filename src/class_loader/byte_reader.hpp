/**
 * @file byte_reader.hpp
 * @author Rive Chen
 * @brief Byte reader for reading bytes from a binary stream: classfile
 * @version 0.2
 * @date 2025-12-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <span>

#include "common/endian.hpp"

namespace jvm::class_loader {
class ByteReader {
 public:
  /**
   * @brief Construct a new ByteReader object
   * @param data Data to read from, not owned by the ByteReader
   */
  explicit ByteReader(std::span<U1> data) : data_(data) {}

  /**
   * @brief Read a value of type T
   * @tparam T Type to read
   * @return Value of type T
   */
  template <typename T>
  T read() {
    checkBounds(sizeof(T));
    T value;
    std::memcpy(&value, data_.data() + pos_, sizeof(T));
    pos_ += sizeof(T);
    return common::swapEndian(value);
  }

  /**
   * @brief Read bytes into a buffer
   * @param buffer Buffer to read into
   * @param count Number of bytes to read
   */
  void readBytes(char* buffer, size_t count) {
    checkBounds(count);
    if (buffer == nullptr) {
      throw std::invalid_argument("ByteReader: buffer is nullptr");
    }
    std::memcpy(buffer, data_.data() + pos_, count);
    pos_ += count;
  }

  /**
   * @brief Read bytes into a vector
   * @param count Number of bytes to read
   * @return Vector of bytes
   */
  std::vector<U1> readBytes(size_t count) {
    checkBounds(count);
    std::vector<U1> buffer(count);
    std::memcpy(buffer.data(), data_.data() + pos_, count);
    pos_ += count;
    return buffer;
  }

 private:
  void checkBounds(size_t n) {
    if (pos_ + n > data_.size()) {
      throw std::out_of_range("ByteReader: read out of bounds");
    }
  }

  // stateful parsing
  std::span<U1> data_;
  size_t        pos_{};
};

}  // namespace jvm::class_loader