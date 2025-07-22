//
// Created by vformato on 7/22/25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("6502 instruction decoding tests") {
  GIVEN("A freshly initialized cpu") {
    BNES::HW::CPU cpu;

    WHEN("We try to decode a LDA immediate instruction") {
      std::vector<uint8_t> bytes = {0xA9, 0x42}; // LDA #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDA_Immediate);
        REQUIRE(instruction.cycles == 2);
        REQUIRE(instruction.operands.size() == 1);
        REQUIRE(instruction.operands[0] == 0x42);
      }
    }
  }
}
