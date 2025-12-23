//
// Created by vformato on 7/22/25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

SCENARIO("6502 instruction decoding tests (all the rest)") {
  GIVEN("A freshly initialized cpu") {
    Bus bus;
    CPU cpu{bus};

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
        using ExpectedInstruction = CPU::TransferRegisterTo<CPU::Register::A, CPU::Register::X>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a TAY instruction") {
      std::vector<uint8_t> bytes = {0xA8}; // TAY
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as a TAY instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::TransferRegisterTo<CPU::Register::A, CPU::Register::Y>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a TXA instruction") {
      std::vector<uint8_t> bytes = {0x8A}; // TXA
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as a TXA instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::TransferRegisterTo<CPU::Register::X, CPU::Register::A>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a TYA instruction") {
      std::vector<uint8_t> bytes = {0x9A}; // TYA
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as a TYA instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::TransferRegisterTo<CPU::Register::Y, CPU::Register::A>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a PHA instruction") {
      std::vector<uint8_t> bytes = {0x48}; // PHA
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as a PHA instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::PushAccumulator;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a PLA instruction") {
      std::vector<uint8_t> bytes = {0x68}; // PLA
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as a PLA instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::PullAccumulator;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
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

    WHEN("We try to decode a BIT zero-page instruction") {
      std::vector<uint8_t> bytes = {0x24, 0x42}; // BIT $42
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::BitTest<AddressingMode::ZeroPage>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 3);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.address == 0x42);
      }
    }

    WHEN("We try to decode a BIT absolute instruction") {
      std::vector<uint8_t> bytes = {0x2C, 0x00, 0x03}; // BIT $0300
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::BitTest<AddressingMode::Absolute>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 4);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x0300);
      }
    }

    WHEN("We try to decode a BNE instruction") {
      std::vector<uint8_t> bytes = {0xD0, 0x10}; // BNE $10
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Branch<Conditional::NotEqual>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == 0x10);
      }

      bytes[1] = 0xFB;
      instruction = cpu.DecodeInstruction(bytes);
      THEN("It should decode negative offsets correctly") {
        using ExpectedInstruction = CPU::Branch<Conditional::NotEqual>;
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
        using ExpectedInstruction = CPU::Branch<Conditional::Equal>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == 0x10);
      }

      bytes[1] = 0xFB;
      instruction = cpu.DecodeInstruction(bytes);
      THEN("It should decode negative offsets correctly") {
        using ExpectedInstruction = CPU::Branch<Conditional::Equal>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == -0x05); // 0xFB is -5 in two's complement
      }
    }

    WHEN("We try to decode a BCC instruction") {
      std::vector<uint8_t> bytes = {0x90, 0x10}; // BCC $10
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Branch<Conditional::CarryClear>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == 0x10);
      }

      bytes[1] = 0xFB;
      instruction = cpu.DecodeInstruction(bytes);
      THEN("It should decode negative offsets correctly") {
        using ExpectedInstruction = CPU::Branch<Conditional::CarryClear>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == -0x05); // 0xFB is -5 in two's complement
      }
    }

    WHEN("We try to decode a BCS instruction") {
      std::vector<uint8_t> bytes = {0xB0, 0x10}; // BCS $10
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Branch<Conditional::CarrySet>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == 0x10);
      }

      bytes[1] = 0xFB;
      instruction = cpu.DecodeInstruction(bytes);
      THEN("It should decode negative offsets correctly") {
        using ExpectedInstruction = CPU::Branch<Conditional::CarrySet>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == -0x05); // 0xFB is -5 in two's complement
      }
    }

    WHEN("We try to decode a BMI instruction") {
      std::vector<uint8_t> bytes = {0x30, 0x10}; // BMI $10
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Branch<Conditional::Minus>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == 0x10);
      }

      bytes[1] = 0xFB;
      instruction = cpu.DecodeInstruction(bytes);
      THEN("It should decode negative offsets correctly") {
        using ExpectedInstruction = CPU::Branch<Conditional::Minus>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == -0x05); // 0xFB is -5 in two's complement
      }
    }

    WHEN("We try to decode a BPL instruction") {
      std::vector<uint8_t> bytes = {0x10, 0x10}; // BPL $10
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Branch<Conditional::Positive>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == 0x10);
      }

      bytes[1] = 0xFB;
      instruction = cpu.DecodeInstruction(bytes);
      THEN("It should decode negative offsets correctly") {
        using ExpectedInstruction = CPU::Branch<Conditional::Positive>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == -0x05); // 0xFB is -5 in two's complement
      }
    }

    WHEN("We try to decode a BVC instruction") {
      std::vector<uint8_t> bytes = {0x50, 0x10}; // BVC $10
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Branch<Conditional::OverflowClear>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == 0x10);
      }

      bytes[1] = 0xFB;
      instruction = cpu.DecodeInstruction(bytes);
      THEN("It should decode negative offsets correctly") {
        using ExpectedInstruction = CPU::Branch<Conditional::OverflowClear>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == -0x05); // 0xFB is -5 in two's complement
      }
    }

    WHEN("We try to decode a BVS instruction") {
      std::vector<uint8_t> bytes = {0x70, 0x10}; // BVS $10
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::Branch<Conditional::OverflowSet>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 2);
        REQUIRE(decoded_instruction.offset == 0x10);
      }

      bytes[1] = 0xFB;
      instruction = cpu.DecodeInstruction(bytes);
      THEN("It should decode negative offsets correctly") {
        using ExpectedInstruction = CPU::Branch<Conditional::OverflowSet>;
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

    WHEN("We try to decode a JSR instruction") {
      std::vector<uint8_t> bytes = {0x20, 0x00, 0x30}; // JSR $3000
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::JumpToSubroutine;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 3);
        REQUIRE(decoded_instruction.address == 0x3000);
      }
    }

    WHEN("We try to decode a RTS instruction") {
      std::vector<uint8_t> bytes = {0x60}; // RTS
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode correctly") {
        using ExpectedInstruction = CPU::ReturnFromSubroutine;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 6);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a CLC instruction") {
      std::vector<uint8_t> bytes = {0x18}; // CLC
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as a CLC instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::ClearStatusFlag<CPU::StatusFlag::Carry>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a CLD instruction") {
      std::vector<uint8_t> bytes = {0xD8}; // CLD
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as a CLD instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::ClearStatusFlag<CPU::StatusFlag::DecimalMode>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a CLI instruction") {
      std::vector<uint8_t> bytes = {0x58}; // CLI
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as a CLI instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::ClearStatusFlag<CPU::StatusFlag::InterruptDisable>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a CLV instruction") {
      std::vector<uint8_t> bytes = {0xB8}; // CLV
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as a CLV instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::ClearStatusFlag<CPU::StatusFlag::Overflow>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a SEC instruction") {
      std::vector<uint8_t> bytes = {0x38}; // SEC
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as a SEC instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::SetStatusFlag<CPU::StatusFlag::Carry>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a SED instruction") {
      std::vector<uint8_t> bytes = {0xF8}; // SED
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as a SED instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::SetStatusFlag<CPU::StatusFlag::DecimalMode>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }

    WHEN("We try to decode a SEI instruction") {
      std::vector<uint8_t> bytes = {0x78}; // SEI
      auto instruction = cpu.DecodeInstruction(bytes);

      THEN("It should decode as a SEI instruction with correct cycle count and size") {
        using ExpectedInstruction = CPU::SetStatusFlag<CPU::StatusFlag::InterruptDisable>;
        REQUIRE(std::holds_alternative<ExpectedInstruction>(instruction));

        auto decoded_instruction = std::get<ExpectedInstruction>(instruction);

        REQUIRE(decoded_instruction.cycles == 2);
        REQUIRE(decoded_instruction.size == 1);
      }
    }
  }
}
