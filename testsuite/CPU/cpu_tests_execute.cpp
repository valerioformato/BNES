//
// Created by vformato on 7/22/25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("6502 instruction execution tests") {
  GIVEN("A freshly initialized cpu") {
    BNES::HW::CPU cpu;

    WHEN("We execute a BRK instruction") {
      BNES::HW::CPU::Instruction instr = {
          .opcode = BNES::HW::OpCode::Break,
          .cycles = 7,
          .operands = {},
      };

      THEN("It should throw a NonMaskableInterrupt exception") { REQUIRE_THROWS(cpu.RunInstruction(instr)); }
    }

    WHEN("We execute a LDA immediate instruction") {
      BNES::HW::CPU::Instruction instr = {
          .opcode = BNES::HW::OpCode::LDA_Immediate,
          .cycles = 2,
          .operands = {0x42},
      };

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the immediate value") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x42);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }

      instr.operands[0] = 0x00; // Change operand to 0
      cpu.RunInstruction(instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x00);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a LDX immediate instruction") {
      BNES::HW::CPU::Instruction instr = {
          .opcode = BNES::HW::OpCode::LDX_Immediate,
          .cycles = 2,
          .operands = {0x42},
      };

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the immediate value") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x42);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }

      instr.operands[0] = 0x00; // Change operand to 0
      cpu.RunInstruction(instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x00);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a TAX instruction") {
      // First, load the accumulator with a value
      BNES::HW::CPU::Instruction lda_instr = {
          .opcode = BNES::HW::OpCode::LDA_Immediate,
          .cycles = 2,
          .operands = {0x42},
      };
      cpu.RunInstruction(lda_instr);

      // Now execute the TAX instruction
      BNES::HW::CPU::Instruction tax_instr = {
          .opcode = BNES::HW::OpCode::TAX,
          .cycles = 2,
          .operands = {},
      };
      cpu.RunInstruction(tax_instr);

      THEN("The X register should be loaded with the value from the accumulator") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x42);
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x42);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }

      lda_instr.operands[0] = 0x00; // Change operand to 0
      cpu.RunInstruction(lda_instr);
      cpu.RunInstruction(tax_instr);

      THEN("The X register should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x00);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a INX instruction") {
      // First, load the X register with a value
      BNES::HW::CPU::Instruction ldx_instr = {
          .opcode = BNES::HW::OpCode::LDX_Immediate,
          .cycles = 2,
          .operands = {0x42},
      };
      cpu.RunInstruction(ldx_instr);

      // Now execute the INX instruction
      BNES::HW::CPU::Instruction inx_instr = {
          .opcode = BNES::HW::OpCode::INX,
          .cycles = 2,
          .operands = {},
      };
      cpu.RunInstruction(inx_instr);

      THEN("The X register should be incremented by 1") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x43);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }

      ldx_instr.operands[0] = 0xFF; // Change operand to 0xFF
      cpu.RunInstruction(ldx_instr);
      cpu.RunInstruction(inx_instr);

      THEN("The X register should wrap around to 0 and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x00);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
    }
  }
}
