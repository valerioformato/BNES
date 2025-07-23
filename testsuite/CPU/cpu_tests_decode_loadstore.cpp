//
// Created by vformato on 7/22/25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("6502 instruction decoding tests (loads and stores)") {
  GIVEN("A freshly initialized cpu") {
    BNES::HW::CPU cpu;

    WHEN("We try to decode a LDA immediate instruction") {
      std::vector<uint8_t> bytes = {0xA9, 0x42}; // LDA #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDA_Immediate);
        REQUIRE(instruction.cycles == 2);
        REQUIRE(instruction.size == 2);
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
        REQUIRE(instruction.size == 2);
        REQUIRE(instruction.operands.size() == 1);
        REQUIRE(instruction.operands[0] == 0x42);
      }
    }

    WHEN("We try to decode a LDY immediate instruction") {
      std::vector<uint8_t> bytes = {0xA0, 0x42}; // LDX #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDY_Immediate);
        REQUIRE(instruction.cycles == 2);
        REQUIRE(instruction.size == 2);
        REQUIRE(instruction.operands.size() == 1);
        REQUIRE(instruction.operands[0] == 0x42);
      }
    }

    WHEN("We try to decode a LDA zero page instruction") {
      std::vector<uint8_t> bytes = {0xA5, 0x42}; // LDA $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDA_ZeroPage);
        REQUIRE(instruction.cycles == 3);
        REQUIRE(instruction.size == 2);
        REQUIRE(instruction.operands.size() == 1);
        REQUIRE(instruction.operands[0] == 0x42);
      }
    }

    WHEN("We try to decode a LDX zero page instruction") {
      std::vector<uint8_t> bytes = {0xA6, 0x42}; // LDX $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDX_ZeroPage);
        REQUIRE(instruction.cycles == 3);
        REQUIRE(instruction.size == 2);
        REQUIRE(instruction.operands.size() == 1);
        REQUIRE(instruction.operands[0] == 0x42);
      }
    }

    WHEN("We try to decode a LDY zero page instruction") {
      std::vector<uint8_t> bytes = {0xA4, 0x42}; // LDY $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDY_ZeroPage);
        REQUIRE(instruction.cycles == 3);
        REQUIRE(instruction.size == 2);
        REQUIRE(instruction.operands.size() == 1);
        REQUIRE(instruction.operands[0] == 0x42);
      }
    }
  }
}
