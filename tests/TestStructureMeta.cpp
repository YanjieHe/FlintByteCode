#include "FlintByteCode/ByteCode.hpp"
#include "TestHelpers.hpp"
#include <catch2/catch.hpp>

using namespace flint_bytecode;
using namespace test_helpers;

// ============================================================
// StructureMeta::Compile
// ============================================================

TEST_CASE("StructureMeta with fields and no vtable", "[StructureMeta]") {
  StructureMeta meta("Point", {"x", "y"}, {});
  ByteCode bc;
  meta.Compile(bc);

  auto &b = bc.GetBytes();
  size_t pos = 0;

  // name
  REQUIRE(read_string(b, pos) == "Point");
  pos += string_size("Point");

  // field count
  REQUIRE(read_u16(b, pos) == 2);
  pos += 2;

  // field names
  REQUIRE(read_string(b, pos) == "x");
  pos += string_size("x");
  REQUIRE(read_string(b, pos) == "y");
  pos += string_size("y");

  // vtable count = 0
  REQUIRE(read_u16(b, pos) == 0);
}

TEST_CASE("StructureMeta with no fields", "[StructureMeta]") {
  StructureMeta meta("Empty", {}, {});
  ByteCode bc;
  meta.Compile(bc);

  auto &b = bc.GetBytes();
  size_t pos = 0;

  REQUIRE(read_string(b, pos) == "Empty");
  pos += string_size("Empty");

  REQUIRE(read_u16(b, pos) == 0); // 0 fields
  pos += 2;

  REQUIRE(read_u16(b, pos) == 0); // 0 vtable entries
}

TEST_CASE("StructureMeta with vtable entries", "[StructureMeta]") {
  std::vector<VTableEntry> vtable;
  vtable.push_back(VTableEntry(0, {1, 2}));
  vtable.push_back(VTableEntry(1, {3}));

  StructureMeta meta("Animal", {"name"}, vtable);
  ByteCode bc;
  meta.Compile(bc);

  auto &b = bc.GetBytes();
  size_t pos = 0;

  REQUIRE(read_string(b, pos) == "Animal");
  pos += string_size("Animal");

  // 1 field
  REQUIRE(read_u16(b, pos) == 1);
  pos += 2;
  REQUIRE(read_string(b, pos) == "name");
  pos += string_size("name");

  // 2 vtable entries
  REQUIRE(read_u16(b, pos) == 2);
  pos += 2;

  // vtable entry 0: interfaceIndex=0, 2 methods [1, 2]
  REQUIRE(read_i32(b, pos) == 0);
  pos += 4;
  REQUIRE(read_u16(b, pos) == 2);
  pos += 2;
  REQUIRE(read_i32(b, pos) == 1);
  pos += 4;
  REQUIRE(read_i32(b, pos) == 2);
  pos += 4;

  // vtable entry 1: interfaceIndex=1, 1 method [3]
  REQUIRE(read_i32(b, pos) == 1);
  pos += 4;
  REQUIRE(read_u16(b, pos) == 1);
  pos += 2;
  REQUIRE(read_i32(b, pos) == 3);
}

// ============================================================
// ByteCodeProgram::Compile - header
// ============================================================

TEST_CASE("ByteCodeProgram header has correct counts", "[ByteCodeProgram]") {
  GlobalVariable gv("g", 0);
  StructureMeta sm("S", {}, {});
  Function fn("f", 0, 0, 0, {}, ByteCode{});
  NativeLibrary nl("lib.so");
  NativeFunction nf("func", 0, 0);

  InterfaceMethodRef imr(0, 1, 2);

  ByteCodeProgram program({gv}, {sm}, {fn}, {nl}, {nf}, {imr}, 0);
  ByteCode bc;
  program.Compile(bc);

  auto &b = bc.GetBytes();
  REQUIRE(read_i32(b, 0) == 1);  // globalVariables count
  REQUIRE(read_i32(b, 4) == 1);  // structures count
  REQUIRE(read_i32(b, 8) == 1);  // functions count
  REQUIRE(read_i32(b, 12) == 1); // nativeLibraries count
  REQUIRE(read_i32(b, 16) == 1); // nativeFunctions count
  REQUIRE(read_i32(b, 20) == 1); // interfaceMethodReferences count
  REQUIRE(read_i32(b, 24) == 0); // entryPoint
}

TEST_CASE("ByteCodeProgram empty program", "[ByteCodeProgram]") {
  ByteCodeProgram program({}, {}, {}, {}, {}, {}, 0);
  ByteCode bc;
  program.Compile(bc);

  // header: 7 x i32 = 28 bytes, all zeros
  REQUIRE(bc.Size() == 28);
  auto &b = bc.GetBytes();
  for (int i = 0; i < 7; i++) {
    REQUIRE(read_i32(b, i * 4) == 0);
  }
}
