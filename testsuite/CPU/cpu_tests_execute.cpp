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
      auto program = ByteArray<1>{0x00};
      auto instr = cpu.DecodeInstruction(program);

      THEN("It should throw a NonMaskableInterrupt exception") {
        REQUIRE_THROWS(cpu.RunInstruction(instr));
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }
    }

    WHEN("We execute a TAX instruction") {
      auto lda_code = ByteArray<2>{0xA9, 0x42}; // LDA #$42
      auto tax_code = ByteArray<1>{0xAA};       // TAX

      auto lda_instr = cpu.DecodeInstruction(lda_code);
      auto tax_instr = cpu.DecodeInstruction(tax_code);

      // First, load the accumulator with a value
      cpu.RunInstruction(lda_instr);
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
      lda_code[1] = 0x00;                          // LDA #$00
      lda_instr = cpu.DecodeInstruction(lda_code); // Change operand to 0
      cpu.RunInstruction(lda_instr);
      cpu.RunInstruction(tax_instr);

      THEN("The X register should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a INX instruction") {
      auto ldx_code = ByteArray<2>{0xA2, 0x42}; // LDX #$42
      auto inx_code = ByteArray<1>{0xE8};       // INX

      auto ldx_instr = cpu.DecodeInstruction(ldx_code);
      auto inx_instr = cpu.DecodeInstruction(inx_code);

      // First, load the X register with a value
      cpu.RunInstruction(ldx_instr);

      // Now execute the INX instruction
      cpu.RunInstruction(inx_instr);

      THEN("The X register should be incremented by 1") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x43);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      ldx_code[1] = 0xFF; // Change operand to 0xFF
      ldx_instr = cpu.DecodeInstruction(ldx_code);
      cpu.RunInstruction(ldx_instr);
      cpu.RunInstruction(inx_instr);

      THEN("The X register should wrap around to 0 and the zero flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }
  }
}
