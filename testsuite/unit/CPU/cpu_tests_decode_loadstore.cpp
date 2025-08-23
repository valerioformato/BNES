//
// Created by vformato on 7/22/25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

SCENARIO("6502 instruction decoding tests (loads)") {
  GIVEN("A freshly initialized cpu") {
    CPU cpu;

    WHEN("We try to decode a LDA immediate instruction") {
      std::vector<uint8_t> bytes = {0xA9, 0x42}; // LDA #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::Immediate>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a LDX immediate instruction") {
      std::vector<uint8_t> bytes = {0xA2, 0x42}; // LDX #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::X, AddressingMode::Immediate>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a LDY immediate instruction") {
      std::vector<uint8_t> bytes = {0xA0, 0x42}; // LDX #$42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::Y, AddressingMode::Immediate>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a LDA zero page instruction") {
      std::vector<uint8_t> bytes = {0xA5, 0x42}; // LDA $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a LDX zero page instruction") {
      std::vector<uint8_t> bytes = {0xA6, 0x42}; // LDX $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::X, AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a LDY zero page instruction") {
      std::vector<uint8_t> bytes = {0xA4, 0x42}; // LDY $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::Y, AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a LDA zero page X instruction") {
      std::vector<uint8_t> bytes = {0xB5, 0x42}; // LDA $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a LDX zero page Y instruction") {
      std::vector<uint8_t> bytes = {0xB6, 0x42}; // LDX $42,Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::X, AddressingMode::ZeroPageY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a LDY zero page X instruction") {
      std::vector<uint8_t> bytes = {0xB4, 0x42}; // LDY $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::Y, AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a LDA absolute instruction") {
      std::vector<uint8_t> bytes = {0xAD, 0x42, 0x01}; // LDA $0042
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x142);
      }
    }

    WHEN("We try to decode a LDX absolute instruction") {
      std::vector<uint8_t> bytes = {0xAE, 0x42, 0x01}; // LDX $0042
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::X, AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x142);
      }
    }

    WHEN("We try to decode a LDY absolute instruction") {
      std::vector<uint8_t> bytes = {0xAC, 0x42, 0x01}; // LDY $0142
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::Y, AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x142);
      }
    }

    WHEN("We try to decode a LDA indexed absolute instruction") {
      std::vector<uint8_t> bytes = {0xBD, 0x42, 0x01}; // LDA $0142,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x142);
      }

      bytes = {0xB9, 0x42, 0x01}; // LDA $0042,Y
      instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::AbsoluteY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x142);
      }
    }

    WHEN("We try to decode a LDX indexed absolute instruction") {
      std::vector<uint8_t> bytes = {0xBE, 0x42, 0x01}; // LDX $0142
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::X, AddressingMode::AbsoluteY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x142);
      }
    }

    WHEN("We try to decode a LDY indexed absolute instruction") {
      std::vector<uint8_t> bytes = {0xBC, 0x42, 0x01}; // LDY $0142
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::Y, AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.value == 0x142);
      }
    }

    WHEN("We try to decode a LDA indexed indirect instruction") {
      std::vector<uint8_t> bytes = {0xA1, 0x42};
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::IndirectX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);
        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }

    WHEN("We try to decode a LDA indirect indexed instruction") {
      std::vector<uint8_t> bytes = {0xB1, 0x42};
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::IndirectY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);
        REQUIRE(decoded_instruction.cycles == 5);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.value == 0x42);
      }
    }
  }
}

SCENARIO("6502 instruction decoding tests (stores)") {
  GIVEN("A freshly initialized cpu") {
    CPU cpu;

    WHEN("We try to decode a STA zero page instruction") {
      std::vector<uint8_t> bytes = {0x85, 0x42}; // STA $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::StoreRegister<CPU::Register::A, AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a STX zero page instruction") {
      std::vector<uint8_t> bytes = {0x86, 0x42}; // STX $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::StoreRegister<CPU::Register::X, AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a STY zero page instruction") {
      std::vector<uint8_t> bytes = {0x84, 0x42}; // STY $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::StoreRegister<CPU::Register::Y, AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a STX zero page Y instruction") {
      std::vector<uint8_t> bytes = {0x96, 0x42}; // STX $42,Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::StoreRegister<CPU::Register::X, AddressingMode::ZeroPageY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a STY zero page X instruction") {
      std::vector<uint8_t> bytes = {0x94, 0x42}; // STY $42,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::StoreRegister<CPU::Register::Y, AddressingMode::ZeroPageX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a STA absolute instruction") {
      std::vector<uint8_t> bytes = {0x8D, 0x42, 0x01}; // STA $0142
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::StoreRegister<CPU::Register::A, AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0142);
      }
    }

    WHEN("We try to decode a STX absolute instruction") {
      std::vector<uint8_t> bytes = {0x8E, 0x42, 0x01}; // STX $0142
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::StoreRegister<CPU::Register::X, AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0142);
      }
    }

    WHEN("We try to decode a STY absolute instruction") {
      std::vector<uint8_t> bytes = {0x8C, 0x42, 0x01}; // STY $0142
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::StoreRegister<CPU::Register::Y, AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0142);
      }
    }

    WHEN("We try to decode a STA indexed absolute instruction") {
      std::vector<uint8_t> bytes = {0x9D, 0x42, 0x01}; // STA $0142,X
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::StoreRegister<CPU::Register::A, AddressingMode::AbsoluteX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 5); // Store takes 5 cycles in absolute X mode
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0142);
      }
    }

    WHEN("We try to decode a STA absolute Y instruction") {
      std::vector<uint8_t> bytes = {0x99, 0x42, 0x01}; // STA $0142,Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::StoreRegister<CPU::Register::A, AddressingMode::AbsoluteY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 5); // Store takes 5 cycles in absolute Y mode
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0142);
      }
    }

    WHEN("We try to decode a STA indexed indirect instruction") {
      std::vector<uint8_t> bytes = {0x81, 0x42}; // STA ($42,X)
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::StoreRegister<CPU::Register::A, AddressingMode::IndirectX>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);
        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a STA indirect indexed instruction") {
      std::vector<uint8_t> bytes = {0x91, 0x42}; // STA ($42),Y
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::StoreRegister<CPU::Register::A, AddressingMode::IndirectY>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);
        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }
  }
}