/**
 * @file byte_reader.hpp
 * @author Rive Chen
 * @brief Byte reader for reading bytes from a binary stream: classfile
 * @version 0.1
 * @date 2025-12-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <istream>

#include "common/endian.hpp"

namespace class_loader {
class ByteReader {
 public:
  explicit ByteReader(std::istream& stream) : stream_(stream) {}

  template <typename T>
  T read() {
    T value;
    stream_.read(reinterpret_cast<char*>(&value), sizeof(T));
    // Assuming stream is big-endian and host is little-endian
    return common::swapEndian(value);
  }

  // Read raw bytes into a buffer
  void readBytes(char* buffer, std::streamsize count) { stream_.read(buffer, count); }

  std::vector<common::U1> readBytes(std::streamsize count) {
    std::vector<common::U1> buffer(count);
    stream_.read(reinterpret_cast<char*>(buffer.data()), count);
    return buffer;
  }

 private:
  std::istream& stream_;
};

}  // namespace class_loader