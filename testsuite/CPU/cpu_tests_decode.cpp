//
// Created by vformato on 7/22/25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

SCENARIO("6502 instruction decoding tests (all the rest)") {
  GIVEN("A freshly initialized cpu") {
    CPU cpu;

    WHEN("We try to decode a BRK instruction") {
      std::vector<uint8_t> bytes = {0x00}; // BRK
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as a TAY instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::Break;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 7);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a TAX instruction") {
      std::vector<uint8_t> bytes = {0xAA}; // TAX
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as a TAX instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::TransferAccumulatorTo<CPU::Register::X>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a TAY instruction") {
      std::vector<uint8_t> bytes = {0xA8}; // TAY
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as a BRK instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::TransferAccumulatorTo<CPU::Register::Y>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a INX instruction") {
      std::vector<uint8_t> bytes = {0xE8}; // INX
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as an INY instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::IncrementRegister<CPU::Register::X>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a INY instruction") {
      std::vector<uint8_t> bytes = {0xC8}; // INY
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as an INX instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::IncrementRegister<CPU::Register::Y>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }
  }
}
