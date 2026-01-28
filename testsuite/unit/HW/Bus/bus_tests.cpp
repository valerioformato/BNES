//
// Created by vformato on 12/27/25.
//

#include "HW/Bus.h"
#include "HW/CPU.h"
#include "HW/PPU.h"

#include <catch2/catch_test_macros.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <numeric>
#include <ranges>

using namespace BNES::HW;

SCENARIO("Bus unit tests") {
  GIVEN("A freshly initialized NES Bus") {
    Bus bus;

    WHEN("We try loading an empty program") {
      REQUIRE(bus.LoadIntoProgramRom({}).has_value());
      THEN("Program memory is empty") {
        REQUIRE(std::ranges::none_of(bus.Rom().program_rom, [](auto byte) { return byte > 0; }));
      }
    }

    WHEN("We try loading a program too big") {
      std::vector<uint8_t> program(Bus::MAX_ADDRESSABLE_ROM_ADDRESS - Bus::ROM_START_REGISTER + 2);
      std::iota(program.begin(), program.end(), 0);

      THEN("We get an error") {
        auto load_result = bus.LoadIntoProgramRom(program);
        REQUIRE(!load_result.has_value());
        REQUIRE(load_result.error().Code() == std::errc::not_enough_memory);
      }
    }
  }
}

class CPUMock : public CPU {
public:
  CPUMock(BNES::HW::Bus &bus) : CPU(bus) {}

  using CPU::ReadFromMemory;
};

SCENARIO("Bus NMI propagation tests", "[Bus][NMI]") {
  GIVEN("A bus with attached CPU") {
    Bus bus;
    CPUMock cpu{bus};
    PPU ppu{bus}; // PPU needed for bus.Tick() calls

    // Setup minimal ROM for NMI vector
    std::vector<uint8_t> rom(0x8000, 0x00);
    rom[0x7FFA] = 0xEF;
    rom[0x7FFB] = 0xBE; // NMI vector -> 0xBEEF
    auto load_result = bus.LoadIntoProgramRom(rom);
    REQUIRE(load_result.has_value());

    WHEN("PropagateNMI is called") {
      auto original_pc = cpu.ProgramCounter();
      auto original_sp = cpu.StackPointer();

      bus.PropagateNMI();

      THEN("The CPU's ProcessNMI should be invoked") {
        REQUIRE(cpu.ProgramCounter() == 0xBEEF);
        REQUIRE(cpu.StackPointer() == original_sp - 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::InterruptDisable) == true);
      }
    }

    WHEN("PropagateNMI is called multiple times") {
      auto original_sp = cpu.StackPointer();

      bus.PropagateNMI();
      auto sp_after_first = cpu.StackPointer();
      REQUIRE(sp_after_first == original_sp - 3);

      bus.PropagateNMI();
      auto sp_after_second = cpu.StackPointer();

      THEN("Each call should invoke CPU's ProcessNMI") { REQUIRE(sp_after_second == sp_after_first - 3); }
    }
  }
}
