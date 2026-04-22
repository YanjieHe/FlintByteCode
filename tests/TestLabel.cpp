#include "FlintByteCode/ByteCode.hpp"
#include "TestHelpers.hpp"
#include <catch2/catch.hpp>

using std::string;
using namespace flint_bytecode;
using namespace test_helpers;

// ============================================================
// Label / CreateLabel / Rewrite
// ============================================================

TEST_CASE("CreateLabel records correct location", "[Label]") {
  ByteCode bc;
  bc.AddOp(OpCode::JUMP);
  Label label = bc.CreateLabel(bc.Size());
  REQUIRE(label.Location() == 1); // after the JUMP opcode
  REQUIRE(label.ID() == 0);
}

TEST_CASE("Rewrite patches bytes at label location", "[Label]") {
  ByteCode bc;
  bc.AddOp(OpCode::JUMP);
  Label label = bc.CreateLabel(bc.Size());
  bc.AddI16(0); // placeholder

  // rewrite the placeholder
  bc.Rewrite(label, 42);
  REQUIRE(read_i16(bc.GetBytes(), label.Location()) == 42);
}

TEST_CASE("Rewrite with negative offset", "[Label]") {
  ByteCode bc;
  bc.AddOp(OpCode::JUMP);
  Label label = bc.CreateLabel(bc.Size());
  bc.AddI16(0);

  bc.Rewrite(label, -10);
  REQUIRE(read_i16(bc.GetBytes(), label.Location()) == -10);
}

TEST_CASE("Multiple labels are independent", "[Label]") {
  ByteCode bc;

  bc.AddOp(OpCode::JUMP);
  Label label1 = bc.CreateLabel(bc.Size());
  bc.AddI16(0);

  bc.AddOp(OpCode::JUMP_IF_TRUE);
  Label label2 = bc.CreateLabel(bc.Size());
  bc.AddI16(0);

  REQUIRE(label1.ID() == 0);
  REQUIRE(label2.ID() == 1);
  REQUIRE(label2.Location() == 4); // after JUMP(1) + i16(2) + JUMP_IF_TRUE(1)

  bc.Rewrite(label1, 100);
  bc.Rewrite(label2, 200);

  REQUIRE(read_i16(bc.GetBytes(), label1.Location()) == 100);
  REQUIRE(read_i16(bc.GetBytes(), label2.Location()) == 200);
}

// End-to-end: JUMP skips over dead code to the correct target
TEST_CASE("test jump (end-to-end)", "[Label]") {
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
  pos += 4; // skip stack, locals, argsSize, constantPoolCount

  // Code size = 9 (JUMP + i16 + PUSH_I32_1 + HALT + PUSH_I32_1 + PUSH_I32_1 +
  // ADD_I32 + HALT)
  REQUIRE(read_u16(bytes, pos) == 9);
  pos += 2;

  // JUMP instruction
  REQUIRE(static_cast<OpCode>(bytes[pos]) == OpCode::JUMP);
  pos += 1;

  // Jump offset (i16): should skip past the dead PUSH_I32_1 + HALT
  int16_t jump_offset = read_i16(bytes, pos);
  size_t after_operand = pos + 2;
  pos += 2;

  // Dead code: PUSH_I32_1, HALT (skipped by jump)
  REQUIRE(static_cast<OpCode>(bytes[pos]) == OpCode::PUSH_I32_1);
  pos += 1;
  REQUIRE(static_cast<OpCode>(bytes[pos]) == OpCode::HALT);
  pos += 1;

  // Verify jump lands exactly here (past the dead code)
  REQUIRE(after_operand + jump_offset == pos);

  // Live code: PUSH_I32_1, PUSH_I32_1, ADD_I32, HALT
  REQUIRE(static_cast<OpCode>(bytes[pos]) == OpCode::PUSH_I32_1);
  pos += 1;
  REQUIRE(static_cast<OpCode>(bytes[pos]) == OpCode::PUSH_I32_1);
  pos += 1;
  REQUIRE(static_cast<OpCode>(bytes[pos]) == OpCode::ADD_I32);
  pos += 1;
  REQUIRE(static_cast<OpCode>(bytes[pos]) == OpCode::HALT);
  pos += 1;

  REQUIRE(pos == bytes.size());
}