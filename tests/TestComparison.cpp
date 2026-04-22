#include "FlintByteCode/ByteCode.hpp"
#include "TestHelpers.hpp"
#include <catch2/catch.hpp>

using std::string;
using namespace flint_bytecode;
using namespace test_helpers;

TEST_CASE("test (70 > 5)", "[Comparison]") {
  ByteCode functionCode;
  functionCode.AddOp(OpCode::PUSH_I32_1BYTE);
  functionCode.AddByte(70);
  functionCode.AddOp(OpCode::PUSH_I32_1BYTE);
  functionCode.AddByte(5);
  functionCode.AddOp(OpCode::GT_I32);
  functionCode.AddOp(OpCode::HALT);
  Function function("Main", 0, 0, 0, {}, functionCode);
  ByteCodeProgram program({}, {}, {function}, {}, {}, {}, 0);
  ByteCode byteCode;
  program.Compile(byteCode);

  auto bytes = byteCode.GetBytes();

  // Program header
  REQUIRE(read_i32(bytes, 8) == 1);  // 1 function
  REQUIRE(read_i32(bytes, 24) == 0); // entry point = 0

  // Function "Main" starts at offset 28
  size_t pos = 28;
  REQUIRE(read_string(bytes, pos) == "Main");
  pos += string_size("Main");
  pos += 4; // skip stack, locals, argsSize, constantPoolCount (all 0)

  // Code size = 6 (2 push-1byte + gt + halt)
  REQUIRE(read_u16(bytes, pos) == 6);
  pos += 2;

  // Instruction sequence
  REQUIRE(static_cast<OpCode>(bytes[pos]) == OpCode::PUSH_I32_1BYTE);
  pos += 1;
  REQUIRE(bytes[pos] == 70);
  pos += 1;
  REQUIRE(static_cast<OpCode>(bytes[pos]) == OpCode::PUSH_I32_1BYTE);
  pos += 1;
  REQUIRE(bytes[pos] == 5);
  pos += 1;
  REQUIRE(static_cast<OpCode>(bytes[pos]) == OpCode::GT_I32);
  pos += 1;
  REQUIRE(static_cast<OpCode>(bytes[pos]) == OpCode::HALT);
  pos += 1;

  REQUIRE(pos == bytes.size());
}

TEST_CASE("test (3.5 < 17.2)", "[Comparison]") {
  ByteCode functionCode;
  functionCode.AddOp(OpCode::PUSH_F32);
  functionCode.AddByte(0);
  functionCode.AddOp(OpCode::PUSH_F32);
  functionCode.AddByte(1);
  functionCode.AddOp(OpCode::LT_F32);
  functionCode.AddOp(OpCode::HALT);

  std::vector<Constant> constantPool;
  constantPool.push_back(
      Constant(ConstantKind::CONSTANT_KIND_F32, float_t(3.5)));
  constantPool.push_back(
      Constant(ConstantKind::CONSTANT_KIND_F32, float_t(17.2)));

  Function function("Main", 0, 0, 0, constantPool, functionCode);
  ByteCodeProgram program({}, {}, {function}, {}, {}, {}, 0);
  ByteCode byteCode;
  program.Compile(byteCode);

  auto bytes = byteCode.GetBytes();

  // Program header
  REQUIRE(read_i32(bytes, 8) == 1);  // 1 function
  REQUIRE(read_i32(bytes, 24) == 0); // entry point = 0

  // Function "Main" starts at offset 28
  size_t pos = 28;
  REQUIRE(read_string(bytes, pos) == "Main");
  pos += string_size("Main");
  pos += 3;                 // skip stack, locals, argsSize
  REQUIRE(bytes[pos] == 2); // 2 constants in pool
  pos += 1;

  // Constant 0: F32 = 3.5
  REQUIRE(bytes[pos] == static_cast<Byte>(ConstantKind::CONSTANT_KIND_F32));
  pos += 1;
  REQUIRE(read_f32(bytes, pos) == Approx(3.5f));
  pos += 4;

  // Constant 1: F32 = 17.2
  REQUIRE(bytes[pos] == static_cast<Byte>(ConstantKind::CONSTANT_KIND_F32));
  pos += 1;
  REQUIRE(read_f32(bytes, pos) == Approx(17.2f));
  pos += 4;

  // Code size = 6
  REQUIRE(read_u16(bytes, pos) == 6);
  pos += 2;

  // Instruction sequence: load constant 0, load constant 1, compare, halt
  REQUIRE(static_cast<OpCode>(bytes[pos]) == OpCode::PUSH_F32);
  pos += 1;
  REQUIRE(bytes[pos] == 0); // constant index 0
  pos += 1;
  REQUIRE(static_cast<OpCode>(bytes[pos]) == OpCode::PUSH_F32);
  pos += 1;
  REQUIRE(bytes[pos] == 1); // constant index 1
  pos += 1;
  REQUIRE(static_cast<OpCode>(bytes[pos]) == OpCode::LT_F32);
  pos += 1;
  REQUIRE(static_cast<OpCode>(bytes[pos]) == OpCode::HALT);
  pos += 1;

  REQUIRE(pos == bytes.size());
}