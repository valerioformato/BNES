//
// Created by vformato on 7/22/25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

template <size_t N> using ByteArray = std::array<uint8_t, N>;

SCENARIO("6502 instruction execution tests (all the rest)") {
  GIVEN("A freshly initialized cpu") {
    CPU cpu;
    auto original_program_counter = cpu.ProgramCounter();

    WHEN("We execute a BRK instruction") {
      THEN("It should throw a NonMaskableInterrupt exception") {
        REQUIRE_THROWS(cpu.RunInstruction(CPU::Break{}));
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }
    }

    WHEN("We execute a TAX instruction") {
      auto load_instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::Immediate>{0x42};
      auto tax_instr = CPU::TransferAccumulatorTo<CPU::Register::X>{};

      // First, load the accumulator with a value
      cpu.RunInstruction(load_instr);
      // Now execute the TAX instruction
      cpu.RunInstruction(tax_instr);

      THEN("The X register should be loaded with the value from the accumulator") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      load_instr.value = 0x00; // LDA #$00
      cpu.RunInstruction(load_instr);
      cpu.RunInstruction(tax_instr);

      THEN("The X register should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a TAY instruction") {
      auto load_instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::Immediate>{0x42};
      auto tay_instr = CPU::TransferAccumulatorTo<CPU::Register::Y>{};

      // First, load the accumulator with a value
      cpu.RunInstruction(load_instr);
      // Now execute the TAY instruction
      cpu.RunInstruction(tay_instr);

      THEN("The Y register should be loaded with the value from the accumulator") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      load_instr.value = 0x00; // LDA #$00
      cpu.RunInstruction(load_instr);
      cpu.RunInstruction(tay_instr);

      THEN("The Y register should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a INX instruction") {
      auto load_instr = CPU::LoadRegister<CPU::Register::X, AddressingMode::Immediate>{0x42};
      auto inx_instr = CPU::IncrementRegister<CPU::Register::X>{};

      // First, load the X register with a value
      cpu.RunInstruction(load_instr);

      // Now execute the INX instruction
      cpu.RunInstruction(inx_instr);

      THEN("The X register should be incremented by 1") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x43);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      load_instr.value = 0xFF; // Change operand to 0xFF
      cpu.RunInstruction(load_instr);
      cpu.RunInstruction(inx_instr);

      THEN("The X register should wrap around to 0 and the zero flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a INY instruction") {
      auto load_instr = CPU::LoadRegister<CPU::Register::Y, AddressingMode::Immediate>{0x42};
      auto iny_instr = CPU::IncrementRegister<CPU::Register::Y>{};

      // First, load the Y register with a value
      cpu.RunInstruction(load_instr);

      // Now execute the INY instruction
      cpu.RunInstruction(iny_instr);

      THEN("The Y register should be incremented by 1") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x43);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      load_instr.value = 0xFF; // Change operand to 0xFF
      cpu.RunInstruction(load_instr);
      cpu.RunInstruction(iny_instr);

      THEN("The Y register should wrap around to 0 and the zero flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }
  }
}
