#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <numeric>

using namespace BNES::HW;

class CPUMock : public CPU {
  // This mock class is used to expose private methods for testing purposes.
public:
  CPUMock(Bus &bus) : CPU(bus) {}
};

SCENARIO("6502 initialization") {
  GIVEN("a new CPU instance") {

    Bus bus;
    CPUMock cpu{bus};

    WHEN("the CPU is initialized") {
      THEN("the CPU registers should be in their default state") {
        auto registers = cpu.Registers();
        REQUIRE(registers[CPU::Register::A] == 0x00); // Accumulator
        REQUIRE(registers[CPU::Register::X] == 0x00); // X register
        REQUIRE(registers[CPU::Register::Y] == 0x00); // X register

        REQUIRE(cpu.StackPointer() == 0xFF);     // Stack pointer starts at 0xFF
        REQUIRE(cpu.ProgramCounter() == 0x8000); // Program counter starts at 0x8000
        REQUIRE(cpu.StatusFlags() == 0x00);      // Default status flags
      }
    }
  }
}
