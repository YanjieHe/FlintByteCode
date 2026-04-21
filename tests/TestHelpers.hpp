#ifndef TEST_HELPERS_HPP
#define TEST_HELPERS_HPP

#include "FlintByteCode/ByteCode.hpp"
#include <bit_converter/bit_converter.hpp>
#include <cstdint>
#include <string>
#include <vector>

namespace test_helpers {

using flint_bytecode::Byte;

inline uint16_t read_u16(const std::vector<Byte> &bytes, size_t offset) {
  return bit_converter::bytes_to_u16(bytes.begin() + offset, true);
}

inline int16_t read_i16(const std::vector<Byte> &bytes, size_t offset) {
  return bit_converter::bytes_to_i16(bytes.begin() + offset, true);
}

inline int32_t read_i32(const std::vector<Byte> &bytes, size_t offset) {
  return bit_converter::bytes_to_i32(bytes.begin() + offset, true);
}

inline int64_t read_i64(const std::vector<Byte> &bytes, size_t offset) {
  return bit_converter::bytes_to_i64(bytes.begin() + offset, true);
}

inline float_t read_f32(const std::vector<Byte> &bytes, size_t offset) {
  return bit_converter::bytes_to_f32(bytes.begin() + offset, true);
}

inline double_t read_f64(const std::vector<Byte> &bytes, size_t offset) {
  return bit_converter::bytes_to_f64(bytes.begin() + offset, true);
}

inline std::string read_string(const std::vector<Byte> &bytes, size_t offset) {
  uint16_t len = read_u16(bytes, offset);
  return std::string(bytes.begin() + offset + 2,
                     bytes.begin() + offset + 2 + len);
}

// Returns total bytes consumed by a string (2-byte length prefix + content)
inline size_t string_size(const std::string &s) { return 2 + s.size(); }

} // namespace test_helpers

#endif // TEST_HELPERS_HPP
