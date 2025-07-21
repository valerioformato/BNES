#include "HW/CPU.h"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>

SCENARIO("6502 initialization") {
  GIVEN("a new CPU instance") {

    BNES::HW::CPU cpu;

    WHEN("the CPU is initialized") {
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

    WHEN("We try loading an empty program") {
      REQUIRE(cpu.LoadProgram(std::vector<uint8_t>{}).has_value());
      THEN("Program memory is empty") {
        REQUIRE(std::ranges::none_of(cpu.ProgramMemory(), [](auto byte) { return byte > 0; }));
      }
    }

    WHEN("We try loading a program too big") {
      std::vector<uint8_t> program(BNES::HW::CPU::PROG_MEM_SIZE + 1);
      std::ranges::iota(program, 0);

      THEN("We get an error") {
        auto load_result = cpu.LoadProgram(program);
        REQUIRE(!load_result.has_value());
        REQUIRE(load_result.error().Code() == std::errc::not_enough_memory);
      }
    }
  }
}
