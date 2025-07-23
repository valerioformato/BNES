//
// Created by vformato on 7/22/25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

template <size_t N> using ByteArray = std::array<uint8_t, N>;

class CPUMock : public BNES::HW::CPU {
  // This mock class is used to expose the WriteToMemory method for testing purposes.
  // In a real scenario, you would not need this as CPU would be used directly.
public:
  using CPU::WriteToMemory;
};

SCENARIO("6502 instruction execution tests (loads and stores)") {
  GIVEN("A freshly initialized cpu") {
    CPUMock cpu;
    auto original_program_counter = cpu.ProgramCounter();

    WHEN("We execute a LDA immediate instruction") {
      auto program = ByteArray<2>{0xA9, 0x42}; // LDA #$42
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the immediate value") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      instr.operands[0] = 0x00; // Change operand to 0
      cpu.RunInstruction(instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      instr.operands[0] = 0x90; // Change operand to something > 0x80
      cpu.RunInstruction(instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a LDX immediate instruction") {
      auto program = ByteArray<2>{0xA2, 0x42};
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the immediate value") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      instr.operands[0] = 0x00; // Change operand to 0
      cpu.RunInstruction(instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      instr.operands[0] = 0x90; // Change operand to something > 0x80
      cpu.RunInstruction(instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a LDY immediate instruction") {
      auto program = ByteArray<2>{0xA0, 0x42};
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the immediate value") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      instr.operands[0] = 0x00; // Change operand to 0
      cpu.RunInstruction(instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      instr.operands[0] = 0x90; // Change operand to something > 0x80
      cpu.RunInstruction(instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a LDA zero page instruction") {
      cpu.WriteToMemory(0x42, 0x22); // Write value 0x22 to zero page address 0x42

      auto program = ByteArray<2>{0xA5, 0x42}; // LDA $42
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x0);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x0);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a LDX zero page instruction") {
      cpu.WriteToMemory(0x42, 0x22); // Write value 0x22 to zero page address 0x42

      auto program = ByteArray<2>{0xA6, 0x42}; // LDX $42
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x0);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x0);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a LDY zero page instruction") {
      cpu.WriteToMemory(0x42, 0x22); // Write value 0x22 to zero page address 0x42

      auto program = ByteArray<2>{0xA4, 0x42}; // LDY $42
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x0);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x0);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
      original_program_counter = cpu.ProgramCounter();
    }
  }
}
