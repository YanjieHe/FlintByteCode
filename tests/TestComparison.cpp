#include <catch2/catch.hpp>
#include <iostream>
#include <fstream>
#include "ByteCode.hpp"

using std::string;
using namespace flint_bytecode;

TEST_CASE("test (70 > 5)", "[Comparison]") {
  ByteCode functionCode;
  functionCode.AddOp(OpCode::PUSH_I32_1BYTE);
  functionCode.AddByte(70);
  functionCode.AddOp(OpCode::PUSH_I32_1BYTE);
  functionCode.AddByte(5);
  functionCode.AddOp(OpCode::GT_I32);
  functionCode.AddOp(OpCode::HALT);
  Function function("Main", 0, 0, 0, {}, functionCode);
  ByteCodeProgram program({}, {}, {function}, {}, {}, 0);
  ByteCode byteCode;
  program.Compile(byteCode);

  std::vector<Byte> expected = {
      0, 0, 0, 0, 0, 0,  0,  0,   0,   0, 0, 1, 0, 0, 0, 0, 0,  0, 0, 0,   0,
      0, 0, 0, 0, 4, 77, 97, 105, 110, 0, 0, 0, 0, 0, 6, 3, 70, 3, 5, 111, 0};
  REQUIRE(byteCode.GetBytes() == expected);
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
  ByteCodeProgram program({}, {}, {function}, {}, {}, 0);
  ByteCode byteCode;
  program.Compile(byteCode);

  std::vector<Byte> expected = {
      0, 0, 0, 0, 0,   0,   0,   0,  0,  0,  0,   1,   0,  0, 0,   0, 0, 0,
      0, 0, 0, 0, 0,   0,   0,   4,  77, 97, 105, 110, 0,  0, 0,   2, 2, 2,
      6, 0, 0, 2, 130, 145, 153, 89, 0,  6,  13,  0,   13, 1, 117, 0};
  REQUIRE(byteCode.GetBytes() == expected);
}