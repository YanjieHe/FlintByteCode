#include <catch2/catch.hpp>
#include "ByteCode.hpp"

using std::string;
using namespace flint_bytecode;

TEST_CASE("test jump", "[Label]") {
  ByteCode functionCode;
  functionCode.AddOp(OpCode::JUMP);
  Label label = functionCode.CreateLabel(functionCode.Size());
  functionCode.AddI16(0);

  functionCode.AddOp(OpCode::PUSH_I32_1);
  functionCode.AddOp(OpCode::HALT);

  functionCode.Rewrite(
      label, functionCode.Size() -
                 static_cast<int32_t>(label.Location() + sizeof(int16_t)));
  functionCode.AddOp(OpCode::PUSH_I32_1);
  functionCode.AddOp(OpCode::PUSH_I32_1);
  functionCode.AddOp(OpCode::ADD_I32);
  functionCode.AddOp(OpCode::HALT);

  Function function("Main", 0, 0, 0, {}, functionCode);
  ByteCodeProgram program({}, {}, {function}, {}, {}, 0);
  ByteCode byteCode;
  program.Compile(byteCode);

  std::vector<Byte> expected = {0,   0, 0,  0,  0,   0,   0, 0,  0, 0, 0, 1,
                                0,   0, 0,  0,  0,   0,   0, 0,  0, 0, 0, 0,
                                0,   4, 77, 97, 105, 110, 0, 0,  0, 0, 0, 9,
                                129, 0, 2,  2,  0,   2,   2, 57, 0};
  REQUIRE(byteCode.GetBytes() == expected);
}