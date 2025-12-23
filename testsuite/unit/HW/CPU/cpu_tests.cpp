#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <numeric>

using namespace BNES::HW;

class CPUMock : public CPU {
  // This mock class is used to expose private methods for testing purposes.
public:
  CPUMock(BNES::HW::Bus &bus) : CPU(bus) {}

  using CPU::ProgramMemory;
};

SCENARIO("6502 initialization") {
  GIVEN("a new CPU instance") {

    BNES::HW::Bus bus;
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

    WHEN("We try loading an empty program") {
      REQUIRE(cpu.LoadProgram({}).has_value());
      THEN("Program memory is empty") {
        REQUIRE(std::ranges::none_of(cpu.ProgramMemory(), [](auto byte) { return byte > 0; }));
      }
    }

    WHEN("We try loading a program too big") {
      std::vector<uint8_t> program(CPU::PROG_MEM_SIZE + 1);
      std::iota(program.begin(), program.end(), 0);

      THEN("We get an error") {
        auto load_result = cpu.LoadProgram(program);
        REQUIRE(!load_result.has_value());
        REQUIRE(load_result.error().Code() == std::errc::not_enough_memory);
      }
    }
  }
}
