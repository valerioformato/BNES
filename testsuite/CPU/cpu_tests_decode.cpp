//
// Created by vformato on 7/22/25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("6502 instruction decoding tests") {
  GIVEN("A freshly initialized cpu") {
    BNES::HW::CPU cpu;

    WHEN("We try to decode a BRK instruction") {
      std::vector<uint8_t> bytes = {0x00}; // BRK
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::Break);
        REQUIRE(instruction.cycles == 7);
        REQUIRE(instruction.operands.empty());
      }
    }

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

    WHEN("We try to decode a LDX immediate instruction") {
      std::vector<uint8_t> bytes = {0xA2, 0x42}; // LDX #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDX_Immediate);
        REQUIRE(instruction.cycles == 2);
        REQUIRE(instruction.operands.size() == 1);
        REQUIRE(instruction.operands[0] == 0x42);
      }
    }

    WHEN("We try to decode a TAX instruction") {
      std::vector<uint8_t> bytes = {0xAA}; // TAX
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::TAX);
        REQUIRE(instruction.cycles == 2);
        REQUIRE(instruction.operands.empty());
      }
    }

    WHEN("We try to decode a INX instruction") {
      std::vector<uint8_t> bytes = {0xE8}; // INX
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::INX);
        REQUIRE(instruction.cycles == 2);
        REQUIRE(instruction.operands.empty());
      }
    }
  }
}
