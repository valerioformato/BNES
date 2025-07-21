#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("6502 initialization") {
  WHEN("the CPU is initialized") {
    GIVEN("a new CPU instance") {
      BNES::HW::CPU cpu;

      THEN("the CPU registers should be in their default state") {
        auto registers = cpu.Registers();
        REQUIRE(registers[BNES::HW::CPU::Register::A] == 0x00); // Accumulator
        REQUIRE(registers[BNES::HW::CPU::Register::X] == 0x00); // X register
        REQUIRE(registers[BNES::HW::CPU::Register::Y] == 0x00); // X register

        REQUIRE(cpu.StackPointer() == 0xFF);     // Stack pointer starts at 0xFF
        REQUIRE(cpu.ProgramCounter() == 0x0000); // Program counter starts at 0x0000
        REQUIRE(cpu.StatusFlags() == 0x00);      // Default status flags
      }
    }
  }
}
