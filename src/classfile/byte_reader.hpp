/**
 * @file byte_reader.hpp
 * @brief Sequential byte reader for parsing class file binary streams.
 *
 * Wraps a `std::span<U1>` with sequential read operations that
 * automatically handle endianness conversion (JVM class files are
 * big-endian). Provides typed reads (read<T>()), byte array reads,
 * and bounds checking.
 *
 */
#pragma once

#include <cstring>
#include <span>
#include <stdexcept>
#include <vector>

#include "utilities/endian.hpp"

namespace jvm::classfile {

/**
 * @brief Sequential reader over a byte buffer with endian conversion.
 *
 * Reads values of type T (typically U1, U2, U4, U8) from a buffer,
 * automatically converting from big-endian (class file format) to
 * host endianness. Tracks position and performs bounds checking.
 */
class ByteReader {
 public:
  /// @brief Construct a ByteReader over the given span.
  /// @param data The byte span to read from (not owned by the reader).
  explicit ByteReader(std::span<U1> data) : data_(data) {}

  /**
   * @brief Read a value of type T from the current position.
   *
   * Advances the position by sizeof(T). Automatically converts
   * from big-endian to host endianness.
   *
   * @tparam T The type to read (typically U1, U2, U4, U8).
   * @return The read value in host endianness.
   * @throws std::out_of_range if there aren't enough bytes left.
   */
  template <typename T>
  T read() {
    checkBounds(sizeof(T));
    T value;
    std::memcpy(&value, data_.data() + pos_, sizeof(T));
    pos_ += sizeof(T);
    return swapEndian(value);
  }

  /**
   * @brief Read raw bytes into a user-provided buffer.
   * @param buffer Destination buffer (must be at least count bytes).
   * @param count Number of bytes to read.
   * @throws std::invalid_argument if buffer is nullptr.
   * @throws std::out_of_range if not enough bytes remain.
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
   * @brief Read raw bytes into a new vector.
   * @param count Number of bytes to read.
   * @return A vector containing the read bytes.
   * @throws std::out_of_range if not enough bytes remain.
   */
  std::vector<U1> readBytes(size_t count) {
    checkBounds(count);
    std::vector<U1> buffer(count);
    std::memcpy(buffer.data(), data_.data() + pos_, count);
    pos_ += count;
    return buffer;
  }

  /// @brief Get current read position.
  size_t getPosition() const { return pos_; }

 private:
  /// @brief Bounds check: ensure @p n bytes remain.
  void checkBounds(size_t n) {
    if (pos_ + n > data_.size()) {
      throw std::out_of_range("ByteReader: read out of bounds");
    }
  }

  std::span<U1> data_;   ///< The underlying byte buffer.
  size_t        pos_{};  ///< Current read position.
};

}  // namespace jvm::classfile