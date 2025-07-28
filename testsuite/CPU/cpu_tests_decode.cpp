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

    WHEN("We try to decode a CPX immediate instruction") {
      std::vector<uint8_t> bytes = {0xE0, 0x42}; // CPX #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::CompareRegister<CPU::Register::X, AddressingMode::Immediate>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a CPX zero-page instruction") {
      std::vector<uint8_t> bytes = {0xE4, 0x42}; // CPX $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::CompareRegister<CPU::Register::X, AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a CPX absolute instruction") {
      std::vector<uint8_t> bytes = {0xEC, 0x00, 0x03}; // CPX $0300
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::CompareRegister<CPU::Register::X, AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a CPY immediate instruction") {
      std::vector<uint8_t> bytes = {0xC0, 0x42}; // CPY #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::CompareRegister<CPU::Register::Y, AddressingMode::Immediate>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a CPY zero-page instruction") {
      std::vector<uint8_t> bytes = {0xC4, 0x42}; // CPY $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::CompareRegister<CPU::Register::Y, AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a CPY absolute instruction") {
      std::vector<uint8_t> bytes = {0xCC, 0x00, 0x03}; // CPY $0300
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::CompareRegister<CPU::Register::Y, AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a BNE instruction") {
      std::vector<uint8_t> bytes = {0xD0, 0x10}; // BNE $10
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::BranchIfNotEqual;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == 0x10);
      }

      bytes[1] = 0xFB;
      instruction = cpu.DecodeInstruction(bytes);
      THEN("It should decode negative offsets correctly") {
        using ExpectedInstruction = CPU::BranchIfNotEqual;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == -0x05); // 0xFB is -5 in two's complement
      }
    }

    WHEN("We try to decode a BEQ instruction") {
      std::vector<uint8_t> bytes = {0xF0, 0x10}; // BEQ $10
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::BranchIfEqual;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == 0x10);
      }

      bytes[1] = 0xFB;
      instruction = cpu.DecodeInstruction(bytes);
      THEN("It should decode negative offsets correctly") {
        using ExpectedInstruction = CPU::BranchIfEqual;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == -0x05); // 0xFB is -5 in two's complement
      }
    }

    WHEN("We try to decode a JMP Absolute instruction") {
      std::vector<uint8_t> bytes = {0x4C, 0x00, 0x30}; // JMP $3000
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Jump<AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x3000);
      }
    }

    WHEN("We try to decode a JMP Indirect instruction") {
      std::vector<uint8_t> bytes = {0x6C, 0x20, 0x30}; // JMP ($3020)
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Jump<AddressingMode::Indirect>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 5);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x3020);
      }
    }
  }
}
