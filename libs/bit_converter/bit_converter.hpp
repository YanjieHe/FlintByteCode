/*
MIT License

Copyright (c) 2021 Yanjie He

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef BIT_CONVERTER_HPP
#define BIT_CONVERTER_HPP
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace bit_converter {

/**
 * @brief Convert the specified 16-bit unsigned integer value to bytes.
 */
template <typename OutputIt>
inline OutputIt u16_to_bytes(uint16_t value, bool is_big_endian,
                             OutputIt output_it) {
  if (is_big_endian) {
    *output_it = static_cast<uint8_t>(value >> 8);
    output_it++;
    *output_it = static_cast<uint8_t>(value & 0xFF);
    output_it++;
  } else {
    *output_it = static_cast<uint8_t>(value & 0xFF);
    output_it++;
    *output_it = static_cast<uint8_t>(value >> 8);
    output_it++;
  }
  return output_it;
}

/**
 * @brief Convert the specified 16-bit signed integer value to bytes.
 */
template <typename OutputIt>
inline OutputIt i16_to_bytes(int16_t value, bool is_big_endian,
                             OutputIt output_it) {
  return u16_to_bytes(static_cast<uint16_t>(value), is_big_endian, output_it);
}

/**
 * @brief Convert the specified 32-bit unsigned integer value to bytes.
 */
template <typename OutputIt>
inline OutputIt u32_to_bytes(uint32_t value, bool is_big_endian,
                             OutputIt output_it) {
  if (is_big_endian) {
    for (int i = 0; i < static_cast<int>(sizeof(uint32_t)); i++) {
      *output_it = static_cast<uint8_t>((value >> (24 - i * 8)) & 0xFF);
      output_it++;
    }
  } else {
    for (int i = static_cast<int>(sizeof(uint32_t)) - 1; i >= 0; i--) {
      *output_it = static_cast<uint8_t>((value >> (24 - i * 8)) & 0xFF);
      output_it++;
    }
  }
  return output_it;
}

/**
 * @brief Convert the specified 32-bit signed integer value to bytes.
 */
template <typename OutputIt>
inline OutputIt i32_to_bytes(int32_t value, bool is_big_endian,
                             OutputIt output_it) {
  return u32_to_bytes(static_cast<uint32_t>(value), is_big_endian, output_it);
}

/**
 * @brief Convert the specified 64-bit unsigned integer value to bytes.
 */
template <typename OutputIt>
inline OutputIt u64_to_bytes(uint64_t value, bool is_big_endian,
                             OutputIt output_it) {
  if (is_big_endian) {
    for (int i = 0; i < static_cast<int>(sizeof(uint64_t)); i++) {
      *output_it = static_cast<uint8_t>((value >> (56 - i * 8)) & 0xFF);
      output_it++;
    }
  } else {
    for (int i = static_cast<int>(sizeof(uint64_t)) - 1; i >= 0; i--) {
      *output_it = static_cast<uint8_t>((value >> (56 - i * 8)) & 0xFF);
      output_it++;
    }
  }
  return output_it;
}

/**
 * @brief Convert the specified 64-bit signed integer value to bytes.
 */
template <typename OutputIt>
inline OutputIt i64_to_bytes(int64_t value, bool is_big_endian,
                             OutputIt output_it) {
  return u64_to_bytes(static_cast<uint64_t>(value), is_big_endian, output_it);
}

template <typename OutputIt>
inline OutputIt create_bytes_from_bits(const std::vector<bool> &bits,
                                       bool is_big_endian, OutputIt output_it) {
  if (is_big_endian) {
    for (int i = 0; i < static_cast<int>(bits.size()) / 8; i++) {
      uint8_t b = 0;
      for (int j = 0; j < 8; j++) {
        b = b + (bits[i * 8 + j] << (7 - j));
      }
      *output_it = b;
      output_it++;
    }

    return output_it;
  } else {
    for (int i = static_cast<int>(bits.size() / 8) - 1; i >= 0; i--) {
      uint8_t b = 0;
      for (int j = 0; j < 8; j++) {
        b = b + (bits[i * 8 + j] << (7 - j));
      }
      *output_it = b;
      output_it++;
    }

    return output_it;
  }
}

/**
 * @brief Convert the specified single-precision floating-point value to bytes.
 */
template <typename OutputIt>
inline OutputIt f32_to_bytes(float_t value, bool is_big_endian,
                             OutputIt output_it) {
  std::vector<bool> bits;
  bits.reserve(sizeof(float_t) * 8);
  bits.push_back(std::signbit(value));

  switch (std::fpclassify(value)) {
  case FP_ZERO: {
    for (size_t i = 1; i < sizeof(float_t) * 8; i++) {
      bits.push_back(false);
    }
    break;
  }
  case FP_SUBNORMAL: {
    for (size_t i = 0; i < 8; i++) {
      bits.push_back(false);
    }
    float_t mantissa = std::ldexp(std::fabs(value), 126);
    for (int i = 0; i < 23; i++) {
      mantissa = mantissa * 2;
      if (mantissa >= 1.0) {
        mantissa = mantissa - 1.0;
        bits.push_back(true);
      } else {
        bits.push_back(false);
      }
    }
    break;
  }
  case FP_NORMAL: {
    int exponent;
    float_t mantissa = std::frexp(std::fabs(value), &exponent);
    exponent = (exponent - 1) + 127;

    for (int i = 0; i < 8; i++) {
      bits.push_back(exponent % 2);
      exponent = exponent / 2;
    }
    std::reverse(bits.begin() + 1, bits.begin() + 1 + 8);
    mantissa = mantissa * 2 - 1;
    for (int i = 0; i < 23; i++) {
      mantissa = mantissa * 2;
      if (mantissa >= 1.0) {
        mantissa = mantissa - 1.0;
        bits.push_back(true);
      } else {
        bits.push_back(false);
      }
    }
    break;
  }
  case FP_INFINITE: {
    for (size_t i = 0; i < 8; i++) {
      bits.push_back(true);
    }
    for (size_t i = 0; i < 23; i++) {
      bits.push_back(false);
    }
    break;
  }
  case FP_NAN: {
    for (size_t i = 0; i < 8; i++) {
      bits.push_back(true);
    }
    bits.push_back(true);
    for (size_t i = 0; i < 22; i++) {
      bits.push_back(false);
    }
    break;
  }
  }

  return create_bytes_from_bits(bits, is_big_endian, output_it);
}

/**
 * @brief Convert the specified double-precision floating-point value to bytes.
 */
template <typename OutputIt>
inline OutputIt f64_to_bytes(double_t value, bool is_big_endian,
                             OutputIt output_it) {
  std::vector<bool> bits;
  bits.reserve(sizeof(double_t) * 8);
  bits.push_back(std::signbit(value));

  switch (std::fpclassify(value)) {
  case FP_ZERO: {
    for (size_t i = 1; i < sizeof(double_t) * 8; i++) {
      bits.push_back(false);
    }
    break;
  }
  case FP_SUBNORMAL: {
    for (size_t i = 0; i < 11; i++) {
      bits.push_back(false);
    }
    double_t mantissa = std::ldexp(std::fabs(value), 1022);
    for (int i = 0; i < 52; i++) {
      mantissa = mantissa * 2;
      if (mantissa >= 1.0) {
        mantissa = mantissa - 1.0;
        bits.push_back(true);
      } else {
        bits.push_back(false);
      }
    }
    break;
  }
  case FP_NORMAL: {
    int exponent;
    double_t mantissa = std::frexp(std::fabs(value), &exponent);
    exponent = (exponent - 1) + 1023;

    for (int i = 0; i < 11; i++) {
      bits.push_back(exponent % 2);
      exponent = exponent / 2;
    }
    std::reverse(bits.begin() + 1, bits.begin() + 1 + 11);
    mantissa = mantissa * 2 - 1;
    for (int i = 0; i < 52; i++) {
      mantissa = mantissa * 2;
      if (mantissa >= 1.0) {
        mantissa = mantissa - 1.0;
        bits.push_back(true);
      } else {
        bits.push_back(false);
      }
    }
    break;
  }
  case FP_INFINITE: {
    for (size_t i = 0; i < 11; i++) {
      bits.push_back(true);
    }
    for (size_t i = 0; i < 52; i++) {
      bits.push_back(false);
    }
    break;
  }
  case FP_NAN: {
    for (size_t i = 0; i < 11; i++) {
      bits.push_back(true);
    }
    bits.push_back(true);
    for (size_t i = 0; i < 51; i++) {
      bits.push_back(false);
    }
    break;
  }
  }

  return create_bytes_from_bits(bits, is_big_endian, output_it);
}

/**
 * @brief Returns a 16-bit unsigned integer converted from two bytes at a
 * specified position in a byte sequence.
 */
template <typename InputIt>
inline uint16_t bytes_to_u16(InputIt input_it, bool is_big_endian) {
  if (is_big_endian) {
    uint16_t result = *input_it;
    input_it++;
    result = (result << 8) + *input_it;
    input_it++;
    return result;
  } else {
    uint16_t result = *input_it;
    input_it++;
    result = result + (static_cast<uint16_t>(*input_it) << 8);
    input_it++;
    return result;
  }
}

/**
 * @brief Returns a 16-bit signed integer converted from two bytes at a
 * specified position in a byte sequence.
 */
template <typename InputIt>
inline int16_t bytes_to_i16(InputIt input_it, bool is_big_endian) {
  return static_cast<int16_t>(bytes_to_u16(input_it, is_big_endian));
}

/**
 * @brief Returns a 32-bit unsigned integer converted from four bytes at a
 * specified position in a byte sequence.
 */
template <typename InputIt>
inline uint32_t bytes_to_u32(InputIt input_it, bool is_big_endian) {
  uint32_t result = 0;
  if (is_big_endian) {
    for (int i = 0; i < static_cast<int>(sizeof(uint32_t)); i++) {
      result = result + (static_cast<uint32_t>(*input_it) << (8 * (4 - 1 - i)));
      input_it++;
    }
    return result;
  } else {
    for (int i = 0; i < static_cast<int>(sizeof(uint32_t)); i++) {
      result = result + (static_cast<uint32_t>(*input_it) << (8 * i));
      input_it++;
    }
    return result;
  }
}

/**
 * @brief Returns a 32-bit signed integer converted from four bytes at a
 * specified position in a byte sequence.
 */
template <typename InputIt>
inline int32_t bytes_to_i32(InputIt input_it, bool is_big_endian) {
  return static_cast<int32_t>(bytes_to_u32(input_it, is_big_endian));
}

/**
 * @brief Returns a 64-bit unsigned integer converted from eight bytes at a
 * specified position in a byte sequence.
 */
template <typename InputIt>
inline uint64_t bytes_to_u64(InputIt input_it, bool is_big_endian) {
  uint64_t result = 0;
  if (is_big_endian) {
    for (int i = 0; i < static_cast<int>(sizeof(uint64_t)); i++) {
      result = result + (static_cast<uint64_t>(*input_it) << (8 * (8 - 1 - i)));
      input_it++;
    }
    return result;
  } else {
    for (int i = 0; i < static_cast<int>(sizeof(uint64_t)); i++) {
      result = result + (static_cast<uint64_t>(*input_it) << (8 * i));
      input_it++;
    }
    return result;
  }
}

/**
 * @brief Returns a 64-bit signed integer converted from eight bytes at a
 * specified position in a byte sequence.
 */
template <typename InputIt>
inline int64_t bytes_to_i64(InputIt input_it, bool is_big_endian) {
  return static_cast<int64_t>(bytes_to_u64(input_it, is_big_endian));
}

/**
 * @brief Returns a single-precision floating-point number converted from four
 * bytes at a specified position in a byte sequence.
 */
template <typename InputIt>
inline float_t bytes_to_f32(InputIt input_it, bool is_big_endian) {
  std::vector<bool> bits;
  bits.reserve(sizeof(float_t) * 8);
  std::vector<uint8_t> bytes;
  bytes.reserve(sizeof(float_t));
  for (int i = 0; i < static_cast<int>(sizeof(float_t)); i++) {
    uint8_t b = *input_it;
    input_it++;
    bytes.push_back(b);
  }
  if (!is_big_endian) {
    std::reverse(bytes.begin(), bytes.end());
  }
  for (uint8_t b : bytes) {
    for (int j = 7; j >= 0; j--) {
      bits.push_back((b >> j) & 1);
    }
  }

  int sign = bits[0] ? (-1) : (+1);
  int raw_exponent = 0;
  for (int i = 0; i < 8; i++) {
    raw_exponent = raw_exponent + bits[1 + i] * (1 << (8 - 1 - i));
  }
  if (raw_exponent == 0) {
    bool is_zero = true;
    for (size_t i = 0; i < 23; i++) {
      if (bits[1 + 8 + i]) {
        is_zero = false;
        break;
      }
    }

    if (is_zero) {
      return std::copysign(0.0f, sign);
    } else {
      float_t mantissa = 0.0f;
      float_t current = 0.5f;
      for (size_t i = 0; i < 23; i++) {
        mantissa = mantissa + bits[1 + 8 + i] * current;
        current = current / 2;
      }

      return std::copysign(std::ldexp(mantissa, -126), sign);
    }
  } else if (raw_exponent == 255) {
    bool is_inf = true;
    for (size_t i = 0; i < 23; i++) {
      if (bits[1 + 8 + i]) {
        is_inf = false;
        break;
      }
    }

    if (is_inf) {
      return std::copysign(std::numeric_limits<float_t>::infinity(), sign);
    } else {
      return std::copysign(std::numeric_limits<float_t>::quiet_NaN(), sign);
    }
  } else {
    int exponent = raw_exponent - 127;
    float_t mantissa = 1.0f;
    float_t current = 0.5f;
    for (int i = 0; i < 23; i++) {
      mantissa = mantissa + bits[1 + 8 + i] * current;
      current = current / 2;
    }
    return std::copysign(std::ldexp(mantissa, exponent), sign);
  }
}

/**
 * @brief Returns a double-precision floating-point number converted from
 * eight bytes at a specified position in a byte sequence.
 */
template <typename InputIt>
inline double_t bytes_to_f64(InputIt input_it, bool is_big_endian) {
  std::vector<bool> bits;
  bits.reserve(sizeof(double_t) * 8);
  std::vector<uint8_t> bytes;
  bytes.reserve(sizeof(double_t));
  for (int i = 0; i < static_cast<int>(sizeof(double_t)); i++) {
    uint8_t b = *input_it;
    input_it++;
    bytes.push_back(b);
  }
  if (!is_big_endian) {
    std::reverse(bytes.begin(), bytes.end());
  }
  for (uint8_t b : bytes) {
    for (int j = 7; j >= 0; j--) {
      bits.push_back((b >> j) & 1);
    }
  }

  int sign = bits[0] ? (-1) : (+1);
  int raw_exponent = 0;
  for (int i = 0; i < 11; i++) {
    raw_exponent = raw_exponent + bits[1 + i] * (1 << (11 - 1 - i));
  }
  if (raw_exponent == 0) {
    bool is_zero = true;
    for (size_t i = 0; i < 52; i++) {
      if (bits[1 + 11 + i]) {
        is_zero = false;
        break;
      }
    }

    if (is_zero) {
      return std::copysign(0.0, sign);
    } else {
      double_t mantissa = 0.0;
      double_t current = 0.5;
      for (size_t i = 0; i < 52; i++) {
        mantissa = mantissa + bits[1 + 11 + i] * current;
        current = current / 2;
      }

      return std::copysign(std::ldexp(mantissa, -1022), sign);
    }
  } else if (raw_exponent == 2047) {
    bool is_inf = true;
    for (size_t i = 0; i < 52; i++) {
      if (bits[1 + 11 + i]) {
        is_inf = false;
        break;
      }
    }

    if (is_inf) {
      return std::copysign(std::numeric_limits<double_t>::infinity(), sign);
    } else {
      return std::copysign(std::numeric_limits<double_t>::quiet_NaN(), sign);
    }
  } else {
    int exponent = raw_exponent - 1023;
    double_t mantissa = 1.0;
    double_t current = 0.5;
    for (int i = 0; i < 52; i++) {
      mantissa = mantissa + bits[1 + 11 + i] * current;
      current = current / 2;
    }
    return std::copysign(std::ldexp(mantissa, exponent), sign);
  }
}

}; // namespace bit_converter
#endif // BIT_CONVERTER_HPP