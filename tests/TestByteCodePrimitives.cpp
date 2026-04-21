#include "FlintByteCode/ByteCode.hpp"
#include "TestHelpers.hpp"
#include <catch2/catch.hpp>
#include <limits>

using namespace flint_bytecode;
using namespace test_helpers;

// ============================================================
// AddOp
// ============================================================

TEST_CASE("AddOp appends single byte with opcode value", "[ByteCode]") {
  ByteCode bc;
  bc.AddOp(OpCode::HALT);
  REQUIRE(bc.Size() == 1);
  REQUIRE(bc.GetBytes()[0] == 0);

  bc.AddOp(OpCode::PUSH_I32_1);
  REQUIRE(bc.Size() == 2);
  REQUIRE(bc.GetBytes()[1] == 2);
}

// ============================================================
// AddByte
// ============================================================

TEST_CASE("AddByte appends a single raw byte", "[ByteCode]") {
  ByteCode bc;
  bc.AddByte(0xFF);
  REQUIRE(bc.Size() == 1);
  REQUIRE(bc.GetBytes()[0] == 0xFF);
}

// ============================================================
// AddI32
// ============================================================

TEST_CASE("AddI32 writes 4 bytes big-endian", "[ByteCode]") {
  ByteCode bc;
  bc.AddI32(0x01020304);
  REQUIRE(bc.Size() == 4);
  auto &b = bc.GetBytes();
  REQUIRE(b[0] == 0x01);
  REQUIRE(b[1] == 0x02);
  REQUIRE(b[2] == 0x03);
  REQUIRE(b[3] == 0x04);
}

TEST_CASE("AddI32 zero", "[ByteCode]") {
  ByteCode bc;
  bc.AddI32(0);
  REQUIRE(read_i32(bc.GetBytes(), 0) == 0);
}

TEST_CASE("AddI32 negative value", "[ByteCode]") {
  ByteCode bc;
  bc.AddI32(-1);
  REQUIRE(read_i32(bc.GetBytes(), 0) == -1);
}

// ============================================================
// AddI64
// ============================================================

TEST_CASE("AddI64 writes 8 bytes big-endian", "[ByteCode]") {
  ByteCode bc;
  bc.AddI64(0x0102030405060708LL);
  REQUIRE(bc.Size() == 8);
  REQUIRE(read_i64(bc.GetBytes(), 0) == 0x0102030405060708LL);
}

TEST_CASE("AddI64 negative", "[ByteCode]") {
  ByteCode bc;
  bc.AddI64(-42);
  REQUIRE(read_i64(bc.GetBytes(), 0) == -42);
}

// ============================================================
// AddF32
// ============================================================

TEST_CASE("AddF32 writes 4 bytes", "[ByteCode]") {
  ByteCode bc;
  bc.AddF32(3.5f);
  REQUIRE(bc.Size() == 4);
  REQUIRE(read_f32(bc.GetBytes(), 0) == Approx(3.5f));
}

TEST_CASE("AddF32 zero", "[ByteCode]") {
  ByteCode bc;
  bc.AddF32(0.0f);
  REQUIRE(read_f32(bc.GetBytes(), 0) == Approx(0.0f));
}

// ============================================================
// AddF64
// ============================================================

TEST_CASE("AddF64 writes 8 bytes", "[ByteCode]") {
  ByteCode bc;
  bc.AddF64(3.14159265358979);
  REQUIRE(bc.Size() == 8);
  REQUIRE(read_f64(bc.GetBytes(), 0) == Approx(3.14159265358979));
}

// ============================================================
// AddU16
// ============================================================

TEST_CASE("AddU16 writes 2 bytes big-endian", "[ByteCode]") {
  ByteCode bc;
  bc.AddU16(0x0102);
  REQUIRE(bc.Size() == 2);
  REQUIRE(read_u16(bc.GetBytes(), 0) == 0x0102);
}

TEST_CASE("AddU16 max value", "[ByteCode]") {
  ByteCode bc;
  bc.AddU16(0xFFFF);
  REQUIRE(read_u16(bc.GetBytes(), 0) == 0xFFFF);
}

// ============================================================
// AddI16
// ============================================================

TEST_CASE("AddI16 writes 2 bytes big-endian", "[ByteCode]") {
  ByteCode bc;
  bc.AddI16(-1);
  REQUIRE(bc.Size() == 2);
  REQUIRE(read_i16(bc.GetBytes(), 0) == -1);
}

// ============================================================
// AddString
// ============================================================

TEST_CASE("AddString writes length-prefixed UTF-8", "[ByteCode]") {
  ByteCode bc;
  bc.AddString("AB");
  REQUIRE(bc.Size() == 4); // 2 bytes length + 2 bytes content
  REQUIRE(read_string(bc.GetBytes(), 0) == "AB");
}

TEST_CASE("AddString empty string", "[ByteCode]") {
  ByteCode bc;
  bc.AddString("");
  REQUIRE(bc.Size() == 2); // just the length prefix
  REQUIRE(read_u16(bc.GetBytes(), 0) == 0);
}

TEST_CASE("AddString preserves content", "[ByteCode]") {
  ByteCode bc;
  bc.AddString("Hello, World!");
  REQUIRE(read_string(bc.GetBytes(), 0) == "Hello, World!");
}

// ============================================================
// Sequential writes
// ============================================================

TEST_CASE("Multiple writes accumulate correctly", "[ByteCode]") {
  ByteCode bc;
  bc.AddI32(100);
  bc.AddI64(200);
  bc.AddString("X");
  REQUIRE(bc.Size() == 4 + 8 + 3);
  REQUIRE(read_i32(bc.GetBytes(), 0) == 100);
  REQUIRE(read_i64(bc.GetBytes(), 4) == 200);
  REQUIRE(read_string(bc.GetBytes(), 12) == "X");
}
