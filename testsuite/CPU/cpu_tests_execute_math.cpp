//
// Created by Valerio Formato on 25-Jul-25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

template <size_t N> using ByteArray = std::array<uint8_t, N>;

class CPUMock : public CPU {
  // This mock class is used to expose private methods for testing purposes.
public:
  using CPU::ReadFromMemory;
  using CPU::SetRegister;
  using CPU::SetStatusFlag;
  using CPU::WriteToMemory;
};

SCENARIO("6502 instruction execution tests (math ops)") {
  GIVEN("A freshly initialized cpu") {
    CPUMock cpu;
    auto original_program_counter = cpu.ProgramCounter();

    WHEN("We execute a ADC immediate instruction") {
      cpu.SetRegister(CPU::Register::A, 0x02);

      auto instr = CPU::AddWithCarry<AddressingMode::Immediate>{0x40};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the immediate value 0x42, with zero flag clear and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x02);
      instr.value = 0xFE;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00, with zero,carry flags set and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x02);
      cpu.SetStatusFlag(CPU::StatusFlag::Carry, true);
      instr.value = 0x80;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x83, with negative flag set and zero,carry flags clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x83);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }
    }
  }
}