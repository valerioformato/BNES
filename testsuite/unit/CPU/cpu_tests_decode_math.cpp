//
// Created by Valerio Formato on 25-Jul-25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

SCENARIO("6502 instruction decoding tests (math ops)") {
  GIVEN("A freshly initialized cpu") {
    CPU cpu;

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

    WHEN("We try to decode a DEX instruction") {
      std::vector<uint8_t> bytes = {0xCA}; // DEX
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as an DEY instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::DecrementRegister<CPU::Register::X>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a DEY instruction") {
      std::vector<uint8_t> bytes = {0x88}; // DEY
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as an DEX instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::DecrementRegister<CPU::Register::Y>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }
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

    WHEN("We try to decode a ADC zero-page,X instruction") {
      std::vector<uint8_t> bytes = {0x75, 0x42}; // ADC $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::AddWithCarry<AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a ADC absolute instruction") {
      std::vector<uint8_t> bytes = {0x6D, 0x00, 0x03}; // ADC $0300
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::AddWithCarry<AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a ADC absolute,X instruction") {
      std::vector<uint8_t> bytes = {0x7D, 0x00, 0x03}; // ADC $0300,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::AddWithCarry<AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a ADC absolute,Y instruction") {
      std::vector<uint8_t> bytes = {0x79, 0x00, 0x03}; // ADC $0300,Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::AddWithCarry<AddressingMode::AbsoluteY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a ADC indirect,X instruction") {
      std::vector<uint8_t> bytes = {0x61, 0x42}; // ADC ($42,X)
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::AddWithCarry<AddressingMode::IndirectX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a ADC indirect,Y instruction") {
      std::vector<uint8_t> bytes = {0x71, 0x42}; // ADC ($42),Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::AddWithCarry<AddressingMode::IndirectY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 5);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a AND immediate instruction") {
      std::vector<uint8_t> bytes = {0x29, 0x42}; // AND #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LogicalAND<AddressingMode::Immediate>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a AND zero-page instruction") {
      std::vector<uint8_t> bytes = {0x25, 0x42}; // AND $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LogicalAND<AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a AND zero-page,X instruction") {
      std::vector<uint8_t> bytes = {0x35, 0x42}; // AND $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LogicalAND<AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a AND absolute instruction") {
      std::vector<uint8_t> bytes = {0x2D, 0x00, 0x03}; // AND $0300
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LogicalAND<AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a AND absolute,X instruction") {
      std::vector<uint8_t> bytes = {0x3D, 0x00, 0x03}; // AND $0300,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LogicalAND<AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a AND absolute,Y instruction") {
      std::vector<uint8_t> bytes = {0x39, 0x00, 0x03}; // AND $0300,Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LogicalAND<AddressingMode::AbsoluteY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a AND (indirect,X) instruction") {
      std::vector<uint8_t> bytes = {0x21, 0x20}; // AND ($20,X)
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LogicalAND<AddressingMode::IndirectX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x20);
      }
    }

    WHEN("We try to decode a AND (indirect),Y instruction") {
      std::vector<uint8_t> bytes = {0x31, 0x20}; // AND ($20),Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LogicalAND<AddressingMode::IndirectY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 5);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x20);
      }
    }
  }
}