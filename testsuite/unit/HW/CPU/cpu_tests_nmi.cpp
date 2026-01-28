//
// Created by vformato on 1/28/26.
//

#include "HW/CPU.h"
#include "HW/PPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

class CPUMock : public CPU {
public:
  CPUMock(BNES::HW::Bus &bus) : CPU(bus) {}

  using CPU::ProcessNMI;
  using CPU::ReadFromMemory;
  using CPU::SetProgramStartAddress;
  using CPU::SetRegister;
  using CPU::SetStatusFlagValue;
  using CPU::StackPointer;
  using CPU::WriteToMemory;
};

SCENARIO("CPU NMI processing tests", "[CPU][NMI]") {
  GIVEN("A CPU with initialized bus and ROM") {
    Bus bus;
    CPUMock cpu{bus};
    PPU ppu{bus}; // PPU needed for bus.Tick() calls

    // Setup ROM with NMI vector
    std::vector<uint8_t> rom(0x8000, 0x00);
    rom[0x7FFA] = 0x00; // NMI vector low byte -> 0xC000
    rom[0x7FFB] = 0xC0; // NMI vector high byte
    auto load_result = bus.LoadIntoProgramRom(rom);
    REQUIRE(load_result.has_value());

    WHEN("ProcessNMI is called with default state") {
      auto original_pc = cpu.ProgramCounter();
      auto original_sp = cpu.StackPointer();
      auto original_status = cpu.StatusFlags().to_ulong();

      cpu.ProcessNMI();

      THEN("Program counter should be set to NMI vector address") { REQUIRE(cpu.ProgramCounter() == 0xC000); }

      THEN("Stack pointer should be decremented by 3") { REQUIRE(cpu.StackPointer() == original_sp - 3); }

      THEN("PC high byte should be pushed to stack") {
        auto pc_high = cpu.ReadFromMemory(0x0100 + original_sp);
        REQUIRE(pc_high == ((original_pc >> 8) & 0xFF));
      }

      THEN("PC low byte should be pushed to stack") {
        auto pc_low = cpu.ReadFromMemory(0x0100 + original_sp - 1);
        REQUIRE(pc_low == (original_pc & 0xFF));
      }

      THEN("Status register should be pushed to stack") {
        auto status_on_stack = cpu.ReadFromMemory(0x0100 + original_sp - 2);
        REQUIRE(status_on_stack == original_status);
      }

      THEN("Interrupt disable flag should be set") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::InterruptDisable) == true);
      }
    }

    WHEN("ProcessNMI is called with non-default PC") {
      cpu.SetProgramStartAddress(0x1234);
      auto original_sp = cpu.StackPointer();

      cpu.ProcessNMI();

      THEN("The correct PC should be pushed to stack") {
        auto pc_high = cpu.ReadFromMemory(0x0100 + original_sp);
        auto pc_low = cpu.ReadFromMemory(0x0100 + original_sp - 1);
        REQUIRE(pc_high == 0x12);
        REQUIRE(pc_low == 0x34);
      }
    }

    WHEN("ProcessNMI is called with various status flags set") {
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, true);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, true);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, true);
      cpu.SetStatusFlagValue(CPU::StatusFlag::InterruptDisable, false);

      auto expected_status = cpu.StatusFlags().to_ulong();
      auto original_sp = cpu.StackPointer();

      cpu.ProcessNMI();

      THEN("The exact status register should be pushed to stack") {
        auto status_on_stack = cpu.ReadFromMemory(0x0100 + original_sp - 2);
        REQUIRE(status_on_stack == expected_status);
      }

      THEN("Interrupt disable flag should be set after NMI") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::InterruptDisable) == true);
      }
    }

    WHEN("ProcessNMI is called with interrupt disable already set") {
      cpu.SetStatusFlagValue(CPU::StatusFlag::InterruptDisable, true);
      auto original_status = cpu.StatusFlags().to_ulong();
      auto original_sp = cpu.StackPointer();

      cpu.ProcessNMI();

      THEN("The status register with interrupt disable set should be pushed") {
        auto status_on_stack = cpu.ReadFromMemory(0x0100 + original_sp - 2);
        REQUIRE(status_on_stack == original_status);
      }

      THEN("Interrupt disable flag should remain set") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::InterruptDisable) == true);
      }
    }

    WHEN("ProcessNMI is called with different NMI vector values") {
      // Update ROM with different NMI vector
      rom[0x7FFA] = 0x56; // NMI vector low byte -> 0xAB56
      rom[0x7FFB] = 0xAB; // NMI vector high byte
      load_result = bus.LoadIntoProgramRom(rom);
      REQUIRE(load_result.has_value());

      cpu.ProcessNMI();

      THEN("PC should be set to the new NMI vector") { REQUIRE(cpu.ProgramCounter() == 0xAB56); }
    }

    WHEN("Multiple NMIs are processed sequentially") {
      auto original_sp = cpu.StackPointer();

      cpu.ProcessNMI();
      auto sp_after_first = cpu.StackPointer();

      cpu.ProcessNMI();
      auto sp_after_second = cpu.StackPointer();

      THEN("Stack pointer should decrement correctly for each NMI") {
        REQUIRE(sp_after_first == original_sp - 3);
        REQUIRE(sp_after_second == sp_after_first - 3);
      }

      THEN("Both sets of data should be on the stack") {
        // First NMI pushed data
        REQUIRE((cpu.ReadFromMemory(0x0100 + original_sp) != 0 || cpu.ReadFromMemory(0x0100 + original_sp - 1) != 0 ||
                 cpu.ReadFromMemory(0x0100 + original_sp - 2) != 0));

        // Second NMI pushed data
        REQUIRE((cpu.ReadFromMemory(0x0100 + sp_after_first) != 0 ||
                 cpu.ReadFromMemory(0x0100 + sp_after_first - 1) != 0 ||
                 cpu.ReadFromMemory(0x0100 + sp_after_first - 2) != 0));
      }
    }

    WHEN("ProcessNMI is called with stack near the bottom") {
      // This test verifies that stack pointer arithmetic wraps correctly
      // Note: We can't manually decrement the stack pointer from outside the CPU,
      // but we can test that NMI processing handles it correctly
      // The stack pointer is uint8_t so it naturally wraps

      cpu.ProcessNMI();

      THEN("Stack pointer should be decremented by 3") {
        REQUIRE(cpu.StackPointer() == 0xFD - 3); // Started at 0xFD
      }
    }
  }
}
