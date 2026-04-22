#include "FlintByteCode/ByteCode.hpp"
#include "TestHelpers.hpp"
#include <catch2/catch.hpp>

using namespace flint_bytecode;
using namespace test_helpers;

// ============================================================
// Constant::Compile for each ConstantKind
// ============================================================

TEST_CASE("Constant I32 compiles kind byte + 4-byte value", "[Constant]") {
  Constant c(ConstantKind::CONSTANT_KIND_I32, int32_t(42));
  ByteCode bc;
  c.Compile(bc);

  auto &b = bc.GetBytes();
  REQUIRE(bc.Size() == 5); // 1 kind + 4 i32
  REQUIRE(b[0] == static_cast<Byte>(ConstantKind::CONSTANT_KIND_I32));
  REQUIRE(read_i32(b, 1) == 42);
}

TEST_CASE("Constant I32 negative", "[Constant]") {
  Constant c(ConstantKind::CONSTANT_KIND_I32, int32_t(-100));
  ByteCode bc;
  c.Compile(bc);
  REQUIRE(read_i32(bc.GetBytes(), 1) == -100);
}

TEST_CASE("Constant I64 compiles kind byte + 8-byte value", "[Constant]") {
  Constant c(ConstantKind::CONSTANT_KIND_I64, int64_t(123456789012345LL));
  ByteCode bc;
  c.Compile(bc);

  REQUIRE(bc.Size() == 9); // 1 kind + 8 i64
  REQUIRE(bc.GetBytes()[0] ==
          static_cast<Byte>(ConstantKind::CONSTANT_KIND_I64));
  REQUIRE(read_i64(bc.GetBytes(), 1) == 123456789012345LL);
}

TEST_CASE("Constant F32 compiles kind byte + 4-byte float", "[Constant]") {
  Constant c(ConstantKind::CONSTANT_KIND_F32, float_t(3.5f));
  ByteCode bc;
  c.Compile(bc);

  REQUIRE(bc.Size() == 5);
  REQUIRE(bc.GetBytes()[0] ==
          static_cast<Byte>(ConstantKind::CONSTANT_KIND_F32));
  REQUIRE(read_f32(bc.GetBytes(), 1) == Approx(3.5f));
}

TEST_CASE("Constant F64 compiles kind byte + 8-byte double", "[Constant]") {
  Constant c(ConstantKind::CONSTANT_KIND_F64, double_t(2.718281828));
  ByteCode bc;
  c.Compile(bc);

  REQUIRE(bc.Size() == 9);
  REQUIRE(bc.GetBytes()[0] ==
          static_cast<Byte>(ConstantKind::CONSTANT_KIND_F64));
  REQUIRE(read_f64(bc.GetBytes(), 1) == Approx(2.718281828));
}

TEST_CASE("Constant STRING compiles kind byte + length-prefixed string",
          "[Constant]") {
  std::string s = "hello";
  Constant c(ConstantKind::CONSTANT_KIND_STRING, s);
  ByteCode bc;
  c.Compile(bc);

  REQUIRE(bc.Size() == 1 + 2 + 5); // kind + u16 len + 5 chars
  REQUIRE(bc.GetBytes()[0] ==
          static_cast<Byte>(ConstantKind::CONSTANT_KIND_STRING));
  REQUIRE(read_string(bc.GetBytes(), 1) == "hello");
}

TEST_CASE("Constant FUNCTION uses i32 encoding", "[Constant]") {
  Constant c(ConstantKind::CONSTANT_KIND_FUNCTION, int32_t(7));
  ByteCode bc;
  c.Compile(bc);

  REQUIRE(bc.Size() == 5);
  REQUIRE(bc.GetBytes()[0] ==
          static_cast<Byte>(ConstantKind::CONSTANT_KIND_FUNCTION));
  REQUIRE(read_i32(bc.GetBytes(), 1) == 7);
}

TEST_CASE("Constant STRUCTURE_META_DATA uses i32 encoding", "[Constant]") {
  Constant c(ConstantKind::CONSTANT_KIND_STRUCTURE_META_DATA, int32_t(3));
  ByteCode bc;
  c.Compile(bc);

  REQUIRE(bc.Size() == 5);
  REQUIRE(read_i32(bc.GetBytes(), 1) == 3);
}

TEST_CASE("Constant GLOBAL_VARIABLE uses i32 encoding", "[Constant]") {
  Constant c(ConstantKind::CONSTANT_KIND_GLOBAL_VARIABLE, int32_t(0));
  ByteCode bc;
  c.Compile(bc);

  REQUIRE(bc.Size() == 5);
  REQUIRE(read_i32(bc.GetBytes(), 1) == 0);
}

TEST_CASE("Constant NATIVE_FUNCTION uses i32 encoding", "[Constant]") {
  Constant c(ConstantKind::CONSTANT_KIND_NATIVE_FUNCTION, int32_t(99));
  ByteCode bc;
  c.Compile(bc);

  REQUIRE(bc.Size() == 5);
  REQUIRE(read_i32(bc.GetBytes(), 1) == 99);
}

TEST_CASE("Constant INTERFACE_METHOD_REFERENCE uses i32 encoding",
          "[Constant]") {
  Constant c(ConstantKind::CONSTANT_KIND_INTERFACE_METHOD_REFERENCE,
             int32_t(42));
  ByteCode bc;
  c.Compile(bc);

  REQUIRE(bc.Size() == 5);
  REQUIRE(bc.GetBytes()[0] ==
          static_cast<Byte>(
              ConstantKind::CONSTANT_KIND_INTERFACE_METHOD_REFERENCE));
  REQUIRE(read_i32(bc.GetBytes(), 1) == 42);
}
