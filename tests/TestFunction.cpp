#include "FlintByteCode/ByteCode.hpp"
#include "TestHelpers.hpp"
#include <catch2/catch.hpp>

using namespace flint_bytecode;
using namespace test_helpers;

// ============================================================
// Function::Compile
// ============================================================

TEST_CASE("Function compiles header fields correctly", "[Function]") {
  Function f("Main", 10, 5, 2, {}, ByteCode{});
  ByteCode bc;
  f.Compile(bc);

  auto &b = bc.GetBytes();
  size_t pos = 0;

  // name: length-prefixed string "Main"
  REQUIRE(read_string(b, pos) == "Main");
  pos += string_size("Main");

  // stack, locals, argsSize
  REQUIRE(b[pos] == 10);
  pos++;
  REQUIRE(b[pos] == 5);
  pos++;
  REQUIRE(b[pos] == 2);
  pos++;

  // constant pool size = 0
  REQUIRE(b[pos] == 0);
  pos++;

  // code size = 0 (u16)
  REQUIRE(read_u16(b, pos) == 0);
}

TEST_CASE("Function with constant pool", "[Function]") {
  std::vector<Constant> pool;
  pool.push_back(Constant(ConstantKind::CONSTANT_KIND_I32, int32_t(42)));

  Function f("test", 1, 1, 0, pool, ByteCode{});
  ByteCode bc;
  f.Compile(bc);

  auto &b = bc.GetBytes();
  size_t pos = string_size("test"); // skip name
  pos += 3;                         // skip stack, locals, argsSize

  // constant pool size = 1
  REQUIRE(b[pos] == 1);
  pos += 1;

  // first constant: kind=I32(0), value=42
  REQUIRE(b[pos] == static_cast<Byte>(ConstantKind::CONSTANT_KIND_I32));
  REQUIRE(read_i32(b, pos + 1) == 42);
}

TEST_CASE("Function with code body", "[Function]") {
  ByteCode code;
  code.AddOp(OpCode::PUSH_I32_0);
  code.AddOp(OpCode::HALT);

  Function f("run", 1, 0, 0, {}, code);
  ByteCode bc;
  f.Compile(bc);

  auto &b = bc.GetBytes();
  size_t pos = string_size("run"); // skip name
  pos += 3;                        // skip stack, locals, argsSize
  pos += 1;                        // skip constant pool size (0)

  // code size should be 2
  REQUIRE(read_u16(b, pos) == 2);
  pos += 2;

  // actual opcodes
  REQUIRE(b[pos] == static_cast<Byte>(OpCode::PUSH_I32_0));
  REQUIRE(b[pos + 1] == static_cast<Byte>(OpCode::HALT));
}

// ============================================================
// GlobalVariable::Compile
// ============================================================

TEST_CASE("GlobalVariable compiles name and initializer offset",
          "[GlobalVariable]") {
  GlobalVariable gv("counter", 42);
  ByteCode bc;
  gv.Compile(bc);

  auto &b = bc.GetBytes();
  REQUIRE(read_string(b, 0) == "counter");
  size_t pos = string_size("counter");
  REQUIRE(read_i32(b, pos) == 42);
}

// ============================================================
// NativeLibrary::Compile
// ============================================================

TEST_CASE("NativeLibrary compiles library path", "[NativeLibrary]") {
  NativeLibrary lib("libmath.so");
  ByteCode bc;
  lib.Compile(bc);

  REQUIRE(read_string(bc.GetBytes(), 0) == "libmath.so");
}

// ============================================================
// NativeFunction::Compile
// ============================================================

TEST_CASE("NativeFunction compiles name, argsSize, and library offset",
          "[NativeFunction]") {
  NativeFunction nf("sqrt", 1, 0);
  ByteCode bc;
  nf.Compile(bc);

  auto &b = bc.GetBytes();
  size_t pos = 0;

  REQUIRE(read_string(b, pos) == "sqrt");
  pos += string_size("sqrt");

  REQUIRE(read_u16(b, pos) == 1); // argsSize
  pos += 2;

  REQUIRE(read_i32(b, pos) == 0); // nativeLibraryOffset
}

// ============================================================
// VTableEntry::Compile
// ============================================================

TEST_CASE("VTableEntry compiles interface index and method indices",
          "[VTableEntry]") {
  VTableEntry entry(5, {10, 20, 30});
  ByteCode bc;
  entry.Compile(bc);

  auto &b = bc.GetBytes();
  size_t pos = 0;

  REQUIRE(read_i32(b, pos) == 5); // interfaceIndex
  pos += 4;

  REQUIRE(read_u16(b, pos) == 3); // method count
  pos += 2;

  REQUIRE(read_i32(b, pos) == 10);
  pos += 4;
  REQUIRE(read_i32(b, pos) == 20);
  pos += 4;
  REQUIRE(read_i32(b, pos) == 30);
}

TEST_CASE("VTableEntry with empty methods", "[VTableEntry]") {
  VTableEntry entry(0, {});
  ByteCode bc;
  entry.Compile(bc);

  auto &b = bc.GetBytes();
  REQUIRE(read_i32(b, 0) == 0);
  REQUIRE(read_u16(b, 4) == 0);
  REQUIRE(bc.Size() == 6); // i32 + u16 fields
}

// ============================================================
// InterfaceMethodRef::Compile
// ============================================================

TEST_CASE(
    "InterfaceMethodRef compiles interfaceIndex, methodIndex, and argsSize",
    "[InterfaceMethodRef]") {
  InterfaceMethodRef ref(3, 7, 2);
  ByteCode bc;
  ref.Compile(bc);

  auto &b = bc.GetBytes();
  size_t pos = 0;

  REQUIRE(read_i32(b, pos) == 3); // interfaceIndex
  pos += 4;

  REQUIRE(read_u16(b, pos) == 7); // methodIndex
  pos += 2;

  REQUIRE(read_u16(b, pos) == 2); // argsSize
  pos += 2;

  REQUIRE(bc.Size() == 8); // i32 + u16 + u16
}

TEST_CASE("InterfaceMethodRef with zero values", "[InterfaceMethodRef]") {
  InterfaceMethodRef ref(0, 0, 0);
  ByteCode bc;
  ref.Compile(bc);

  auto &b = bc.GetBytes();
  REQUIRE(read_i32(b, 0) == 0);
  REQUIRE(read_u16(b, 4) == 0);
  REQUIRE(read_u16(b, 6) == 0);
  REQUIRE(bc.Size() == 8);
}
