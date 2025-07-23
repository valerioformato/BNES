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

    WHEN("We try to decode a LDA zero page X instruction") {
      std::vector<uint8_t> bytes = {0xB5, 0x42}; // LDA $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDA_ZeroPageX);
        REQUIRE(instruction.cycles == 4);
        REQUIRE(instruction.size == 2);
        REQUIRE(instruction.operands.size() == 1);
        REQUIRE(instruction.operands[0] == 0x42);
      }
    }

    WHEN("We try to decode a LDX zero page Y instruction") {
      std::vector<uint8_t> bytes = {0xB6, 0x42}; // LDX $42,Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDX_ZeroPageY);
        REQUIRE(instruction.cycles == 4);
        REQUIRE(instruction.size == 2);
        REQUIRE(instruction.operands.size() == 1);
        REQUIRE(instruction.operands[0] == 0x42);
      }
    }

    WHEN("We try to decode a LDY zero page X instruction") {
      std::vector<uint8_t> bytes = {0xB4, 0x42}; // LDY $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDY_ZeroPageX);
        REQUIRE(instruction.cycles == 4);
        REQUIRE(instruction.size == 2);
        REQUIRE(instruction.operands.size() == 1);
        REQUIRE(instruction.operands[0] == 0x42);
      }
    }

    WHEN("We try to decode a LDA absolute instruction") {
      std::vector<uint8_t> bytes = {0xAD, 0x42, 0x01}; // LDA $0042
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDA_Absolute);
        REQUIRE(instruction.cycles == 4);
        REQUIRE(instruction.size == 3);
        REQUIRE(instruction.operands.size() == 2);
        REQUIRE(instruction.operands[0] == 0x42);
        REQUIRE(instruction.operands[1] == 0x01);
      }
    }

    WHEN("We try to decode a LDX absolute instruction") {
      std::vector<uint8_t> bytes = {0xAE, 0x42, 0x01}; // LDX $0042
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDX_Absolute);
        REQUIRE(instruction.cycles == 4);
        REQUIRE(instruction.size == 3);
        REQUIRE(instruction.operands.size() == 2);
        REQUIRE(instruction.operands[0] == 0x42);
        REQUIRE(instruction.operands[1] == 0x01);
      }
    }

    WHEN("We try to decode a LDY absolute instruction") {
      std::vector<uint8_t> bytes = {0xAC, 0x42, 0x01}; // LDY $0142
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDY_Absolute);
        REQUIRE(instruction.cycles == 4);
        REQUIRE(instruction.size == 3);
        REQUIRE(instruction.operands.size() == 2);
        REQUIRE(instruction.operands[0] == 0x42);
        REQUIRE(instruction.operands[1] == 0x01);
      }
    }

    WHEN("We try to decode a LDA indexed absolute instruction") {
      std::vector<uint8_t> bytes = {0xBD, 0x42, 0x01}; // LDA $0142,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDA_AbsoluteX);
        REQUIRE(instruction.cycles == 4);
        REQUIRE(instruction.size == 3);
        REQUIRE(instruction.operands.size() == 2);
        REQUIRE(instruction.operands[0] == 0x42);
        REQUIRE(instruction.operands[1] == 0x01);
      }

      bytes = {0xB9, 0x42, 0x01}; // LDA $0042,Y
      instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDA_AbsoluteY);
        REQUIRE(instruction.cycles == 4);
        REQUIRE(instruction.size == 3);
        REQUIRE(instruction.operands.size() == 2);
        REQUIRE(instruction.operands[0] == 0x42);
        REQUIRE(instruction.operands[1] == 0x01);
      }
    }

    WHEN("We try to decode a LDX absolute instruction") {
      std::vector<uint8_t> bytes = {0xBE, 0x42, 0x01}; // LDX $0142,Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDX_AbsoluteY);
        REQUIRE(instruction.cycles == 4);
        REQUIRE(instruction.size == 3);
        REQUIRE(instruction.operands.size() == 2);
        REQUIRE(instruction.operands[0] == 0x42);
        REQUIRE(instruction.operands[1] == 0x01);
      }
    }

    WHEN("We try to decode a LDY absolute instruction") {
      std::vector<uint8_t> bytes = {0xBC, 0x42, 0x01}; // LDY $0042,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        REQUIRE(instruction.opcode == BNES::HW::OpCode::LDY_AbsoluteX);
        REQUIRE(instruction.cycles == 4);
        REQUIRE(instruction.size == 3);
        REQUIRE(instruction.operands.size() == 2);
        REQUIRE(instruction.operands[0] == 0x42);
        REQUIRE(instruction.operands[1] == 0x01);
      }
    }
  }
}
