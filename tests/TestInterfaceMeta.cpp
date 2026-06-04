#include "FlintByteCode/ByteCode.hpp"
#include "TestHelpers.hpp"
#include <catch2/catch.hpp>

using namespace flint_bytecode;
using namespace test_helpers;

// ============================================================
// InterfaceMeta::Compile
// ============================================================

TEST_CASE("InterfaceMeta compiles interfaceIndex, name and method names",
          "[InterfaceMeta]") {
  InterfaceMeta meta(1, "Drawable", {"draw", "resize"});
  ByteCode bc;
  meta.Compile(bc);

  auto &b = bc.GetBytes();
  size_t pos = 0;

  // interfaceIndex
  REQUIRE(read_i32(b, pos) == 1);
  pos += 4;

  // name
  REQUIRE(read_string(b, pos) == "Drawable");
  pos += string_size("Drawable");

  // method count
  REQUIRE(read_u16(b, pos) == 2);
  pos += 2;

  // method names
  REQUIRE(read_string(b, pos) == "draw");
  pos += string_size("draw");
  REQUIRE(read_string(b, pos) == "resize");
  pos += string_size("resize");

  REQUIRE(pos == static_cast<size_t>(bc.Size()));
}

TEST_CASE("InterfaceMeta with no methods", "[InterfaceMeta]") {
  InterfaceMeta meta(0, "Marker", {});
  ByteCode bc;
  meta.Compile(bc);

  auto &b = bc.GetBytes();
  size_t pos = 0;

  REQUIRE(read_i32(b, pos) == 0);
  pos += 4;

  REQUIRE(read_string(b, pos) == "Marker");
  pos += string_size("Marker");

  // 0 methods
  REQUIRE(read_u16(b, pos) == 0);
  pos += 2;

  REQUIRE(pos == static_cast<size_t>(bc.Size()));
}

TEST_CASE("InterfaceMeta with single method", "[InterfaceMeta]") {
  InterfaceMeta meta(5, "Runnable", {"run"});
  ByteCode bc;
  meta.Compile(bc);

  auto &b = bc.GetBytes();
  size_t pos = 0;

  REQUIRE(read_i32(b, pos) == 5);
  pos += 4;

  REQUIRE(read_string(b, pos) == "Runnable");
  pos += string_size("Runnable");

  REQUIRE(read_u16(b, pos) == 1);
  pos += 2;

  REQUIRE(read_string(b, pos) == "run");
  pos += string_size("run");

  REQUIRE(pos == static_cast<size_t>(bc.Size()));
}
