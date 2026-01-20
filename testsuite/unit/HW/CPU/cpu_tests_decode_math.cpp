//
// Created by Valerio Formato on 25-Jul-25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

SCENARIO("6502 instruction decoding tests (math ops)", "[Decode][Math]") {
  GIVEN("A freshly initialized cpu") {
    Bus bus;
    CPU cpu{bus};

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

    WHEN("We try to decode a ASL accumulator instruction") {
      std::vector<uint8_t> bytes = {0x0A}; // ASL A
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ShiftLeft<AddressingMode::Accumulator>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
        REQUIRE(decoded_instruction.address == 0x00);
      }
    }

    WHEN("We try to decode a ASL zero-page instruction") {
      std::vector<uint8_t> bytes = {0x06, 0x42}; // ASL $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ShiftLeft<AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 5);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a ASL zero-page,X instruction") {
      std::vector<uint8_t> bytes = {0x16, 0x42}; // ASL $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ShiftLeft<AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a ASL absolute instruction") {
      std::vector<uint8_t> bytes = {0x0E, 0x00, 0x03}; // ASL $0300
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ShiftLeft<AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0300);
      }
    }

    WHEN("We try to decode a ASL absolute,X instruction") {
      std::vector<uint8_t> bytes = {0x1E, 0x00, 0x03}; // ASL $0300,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ShiftLeft<AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 7);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0300);
      }
    }

    WHEN("We try to decode a LSR accumulator instruction") {
      std::vector<uint8_t> bytes = {0x4A}; // LSR A
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ShiftRight<AddressingMode::Accumulator>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
        REQUIRE(decoded_instruction.address == 0x00);
      }
    }

    WHEN("We try to decode a LSR zero-page instruction") {
      std::vector<uint8_t> bytes = {0x46, 0x42}; // LSR $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ShiftRight<AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 5);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a LSR zero-page,X instruction") {
      std::vector<uint8_t> bytes = {0x56, 0x42}; // LSR $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ShiftRight<AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a LSR absolute instruction") {
      std::vector<uint8_t> bytes = {0x4E, 0x00, 0x03}; // LSR $0300
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ShiftRight<AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0300);
      }
    }

    WHEN("We try to decode a LSR absolute,X instruction") {
      std::vector<uint8_t> bytes = {0x5E, 0x00, 0x03}; // LSR $0300,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ShiftRight<AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 7);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0300);
      }
    }

    WHEN("We try to decode a ROR accumulator instruction") {
      std::vector<uint8_t> bytes = {0x6A}; // ROR A
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::RotateRight<AddressingMode::Accumulator>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a ROR zero-page instruction") {
      std::vector<uint8_t> bytes = {0x66, 0x42}; // ROR $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::RotateRight<AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 5);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a ROR zero-page,X instruction") {
      std::vector<uint8_t> bytes = {0x76, 0x42}; // ROR $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::RotateRight<AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a ROR absolute instruction") {
      std::vector<uint8_t> bytes = {0x6E, 0x00, 0x03}; // ROR $0300
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::RotateRight<AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0300);
      }
    }

    WHEN("We try to decode a ROR absolute,X instruction") {
      std::vector<uint8_t> bytes = {0x7E, 0x00, 0x03}; // ROR $0300,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::RotateRight<AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 7);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0300);
      }
    }

    WHEN("We try to decode a ROL accumulator instruction") {
      std::vector<uint8_t> bytes = {0x2A}; // ROL A
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::RotateLeft<AddressingMode::Accumulator>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a ROL zero-page instruction") {
      std::vector<uint8_t> bytes = {0x26, 0x42}; // ROL $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::RotateLeft<AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 5);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a ROL zero-page,X instruction") {
      std::vector<uint8_t> bytes = {0x36, 0x42}; // ROL $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::RotateLeft<AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a ROL absolute instruction") {
      std::vector<uint8_t> bytes = {0x2E, 0x00, 0x03}; // ROL $0300
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::RotateLeft<AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0300);
      }
    }

    WHEN("We try to decode a ROL absolute,X instruction") {
      std::vector<uint8_t> bytes = {0x3E, 0x00, 0x03}; // ROL $0300,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::RotateLeft<AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 7);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0300);
      }
    }

    WHEN("We try to decode a DEC zero-page instruction") {
      std::vector<uint8_t> bytes = {0xC6, 0x42}; // DEC $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Decrement<AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 5);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a DEC zero-page,X instruction") {
      std::vector<uint8_t> bytes = {0xD6, 0x42}; // DEC $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Decrement<AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a DEC absolute instruction") {
      std::vector<uint8_t> bytes = {0xCE, 0x00, 0x03}; // DEC $0300
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Decrement<AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0300);
      }
    }

    WHEN("We try to decode a DEC absolute,X instruction") {
      std::vector<uint8_t> bytes = {0xDE, 0x00, 0x03}; // DEC $0300,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Decrement<AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 7);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0300);
      }
    }

    WHEN("We try to decode a EOR immediate instruction") {
      std::vector<uint8_t> bytes = {0x49, 0x42}; // EOR #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ExclusiveOR<AddressingMode::Immediate>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a EOR zero-page instruction") {
      std::vector<uint8_t> bytes = {0x45, 0x42}; // EOR $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ExclusiveOR<AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a EOR zero-page,X instruction") {
      std::vector<uint8_t> bytes = {0x55, 0x42}; // EOR $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ExclusiveOR<AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a EOR absolute instruction") {
      std::vector<uint8_t> bytes = {0x4D, 0x00, 0x03}; // EOR $0300
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ExclusiveOR<AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a EOR absolute,X instruction") {
      std::vector<uint8_t> bytes = {0x5D, 0x00, 0x03}; // EOR $0300,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ExclusiveOR<AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a EOR absolute,Y instruction") {
      std::vector<uint8_t> bytes = {0x59, 0x00, 0x03}; // EOR $0300,Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ExclusiveOR<AddressingMode::AbsoluteY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a EOR (indirect,X) instruction") {
      std::vector<uint8_t> bytes = {0x41, 0x20}; // EOR ($20,X)
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ExclusiveOR<AddressingMode::IndirectX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x20);
      }
    }

    WHEN("We try to decode a EOR (indirect),Y instruction") {
      std::vector<uint8_t> bytes = {0x51, 0x20}; // EOR ($20),Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ExclusiveOR<AddressingMode::IndirectY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 5);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x20);
      }
    }

    WHEN("We try to decode an INC zero-page instruction") {
      std::vector<uint8_t> bytes = {0xE6, 0x42}; // INC $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Increment<AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 5);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode an INC zero-page,X instruction") {
      std::vector<uint8_t> bytes = {0xF6, 0x42}; // INC $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Increment<AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode an INC absolute instruction") {
      std::vector<uint8_t> bytes = {0xEE, 0x00, 0x03}; // INC $0300
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Increment<AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0300);
      }
    }

    WHEN("We try to decode an INC absolute,X instruction") {
      std::vector<uint8_t> bytes = {0xFE, 0x00, 0x03}; // INC $0300,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Increment<AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 7);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0300);
      }
    }

    WHEN("We try to decode a SBC immediate instruction") {
      std::vector<uint8_t> bytes = {0xE9, 0x42}; // SBC #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::SubtractWithCarry<AddressingMode::Immediate>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode an undocumented SBC immediate instruction (0xEB)") {
      std::vector<uint8_t> bytes = {0xEB, 0x42}; // SBC #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::SubtractWithCarry<AddressingMode::Immediate>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a SBC zero-page instruction") {
      std::vector<uint8_t> bytes = {0xE5, 0x42}; // SBC $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::SubtractWithCarry<AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a SBC zero-page,X instruction") {
      std::vector<uint8_t> bytes = {0xF5, 0x42}; // SBC $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::SubtractWithCarry<AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a SBC absolute instruction") {
      std::vector<uint8_t> bytes = {0xED, 0x00, 0x03}; // SBC $0300
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::SubtractWithCarry<AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a SBC absolute,X instruction") {
      std::vector<uint8_t> bytes = {0xFD, 0x00, 0x03}; // SBC $0300,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::SubtractWithCarry<AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a SBC absolute,Y instruction") {
      std::vector<uint8_t> bytes = {0xF9, 0x00, 0x03}; // SBC $0300,Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::SubtractWithCarry<AddressingMode::AbsoluteY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a SBC indirect,X instruction") {
      std::vector<uint8_t> bytes = {0xE1, 0x42}; // SBC ($42,X)
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::SubtractWithCarry<AddressingMode::IndirectX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a SBC indirect,Y instruction") {
      std::vector<uint8_t> bytes = {0xF1, 0x42}; // SBC ($42),Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::SubtractWithCarry<AddressingMode::IndirectY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 5);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode an ORA immediate instruction") {
      std::vector<uint8_t> bytes = {0x09, 0x42}; // ORA #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::BitwiseOR<AddressingMode::Immediate>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode an ORA zero-page instruction") {
      std::vector<uint8_t> bytes = {0x05, 0x42}; // ORA $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::BitwiseOR<AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode an ORA zero-page,X instruction") {
      std::vector<uint8_t> bytes = {0x15, 0x42}; // ORA $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::BitwiseOR<AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode an ORA absolute instruction") {
      std::vector<uint8_t> bytes = {0x0D, 0x00, 0x03}; // ORA $0300
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::BitwiseOR<AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode an ORA absolute,X instruction") {
      std::vector<uint8_t> bytes = {0x1D, 0x00, 0x03}; // ORA $0300,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::BitwiseOR<AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode an ORA absolute,Y instruction") {
      std::vector<uint8_t> bytes = {0x19, 0x00, 0x03}; // ORA $0300,Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::BitwiseOR<AddressingMode::AbsoluteY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode an ORA (indirect,X) instruction") {
      std::vector<uint8_t> bytes = {0x01, 0x20}; // ORA ($20,X)
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::BitwiseOR<AddressingMode::IndirectX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x20);
      }
    }

    WHEN("We try to decode an ORA (indirect),Y instruction") {
      std::vector<uint8_t> bytes = {0x11, 0x20}; // ORA ($20),Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::BitwiseOR<AddressingMode::IndirectY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 5);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x20);
      }
    }

    WHEN("We try to decode a CMP zero-page,X instruction") {
      std::vector<uint8_t> bytes = {0xD5, 0x42}; // CMP $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::CompareRegister<CPU::Register::A, AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a CMP absolute,X instruction") {
      std::vector<uint8_t> bytes = {0xDD, 0x00, 0x03}; // CMP $0300,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::CompareRegister<CPU::Register::A, AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a CMP absolute,Y instruction") {
      std::vector<uint8_t> bytes = {0xD9, 0x00, 0x03}; // CMP $0300,Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::CompareRegister<CPU::Register::A, AddressingMode::AbsoluteY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x0300);
      }
    }

    WHEN("We try to decode a DOP immediate instruction (0x80)") {
      std::vector<uint8_t> bytes = {0x80, 0x42}; // DOP #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::DoubleNoOperation<AddressingMode::Immediate>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a DOP immediate instruction (0x82)") {
      std::vector<uint8_t> bytes = {0x82, 0x55}; // DOP #$55
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::DoubleNoOperation<AddressingMode::Immediate>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x55);
      }
    }

    WHEN("We try to decode a DOP zero-page instruction (0x04)") {
      std::vector<uint8_t> bytes = {0x04, 0x50}; // DOP $50
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::DoubleNoOperation<AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x50);
      }
    }

    WHEN("We try to decode a DOP zero-page instruction (0x44)") {
      std::vector<uint8_t> bytes = {0x44, 0x60}; // DOP $60
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::DoubleNoOperation<AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x60);
      }
    }

    WHEN("We try to decode a DOP zero-page,X instruction (0x14)") {
      std::vector<uint8_t> bytes = {0x14, 0x70}; // DOP $70,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::DoubleNoOperation<AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x70);
      }
    }

    WHEN("We try to decode a DOP zero-page,X instruction (0xD4)") {
      std::vector<uint8_t> bytes = {0xD4, 0x80}; // DOP $80,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::DoubleNoOperation<AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x80);
      }
    }

    WHEN("We try to decode a TOP absolute instruction (0x0C)") {
      std::vector<uint8_t> bytes = {0x0C, 0x34, 0x12}; // TOP $1234
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::TripleNoOperation<AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x1234);
      }
    }

    WHEN("We try to decode a TOP absolute,X instruction (0x1C)") {
      std::vector<uint8_t> bytes = {0x1C, 0xAB, 0xCD}; // TOP $CDAB,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::TripleNoOperation<AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0xCDAB);
      }
    }

    WHEN("We try to decode a TOP absolute,X instruction (0x7C)") {
      std::vector<uint8_t> bytes = {0x7C, 0x00, 0x80}; // TOP $8000,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::TripleNoOperation<AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x8000);
      }
    }

    WHEN("We try to decode a TOP absolute,X instruction (0xFC)") {
      std::vector<uint8_t> bytes = {0xFC, 0xFF, 0xFF}; // TOP $FFFF,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::TripleNoOperation<AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0xFFFF);
      }
    }
  }
}
