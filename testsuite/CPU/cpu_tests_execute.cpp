//
// Created by vformato on 7/22/25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("6502 instruction execution tests") {
  GIVEN("A freshly initialized cpu") {
    BNES::HW::CPU cpu;

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
  }
}
