//
// Created by Valerio Formato on 25-Jul-25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

SCENARIO("6502 instruction decoding tests (math ops)") {
  GIVEN("A freshly initialized cpu") {
    CPU cpu;

    WHEN("We try to decode a ADC immediate instruction") {
      std::vector<uint8_t> bytes = {0x69, 0x42}; // ADC #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::AddWithCarry<AddressingMode::Immediate>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a ADC zero-page instruction") {
      std::vector<uint8_t> bytes = {0x65, 0x42}; // ADC $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::AddWithCarry<AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }
  }
}