//
// Created by vformato on 1/28/26.
//
// NMI Integration Tests - Strategy 2
// Tests the complete NMI flow through PPU→Bus→CPU using real components

#include "HW/Bus.h"
#include "HW/CPU.h"
#include "HW/PPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

class CPUMock : public CPU {
public:
  CPUMock(BNES::HW::Bus &bus) : CPU(bus) {}

  using CPU::ReadFromMemory;
  using CPU::SetProgramStartAddress;
};

class PPUMock : public PPU {
public:
  using PPU::IsInVblank;
  using PPU::PPU;
  using PPU::Tick;
  using PPU::WritePPUCTRL;
};

// Helper to execute one CPU instruction
void ExecuteOneInstruction(CPUMock &cpu) {
  auto pc = cpu.ProgramCounter();

  // Read instruction bytes
  std::array<uint8_t, 3> instr_bytes{};
  for (size_t i = 0; i < 3; ++i) {
    instr_bytes[i] = cpu.ReadFromMemory(pc + i);
  }

  std::span bytes(instr_bytes.begin(), 3);
  cpu.RunInstruction(cpu.DecodeInstruction(bytes));
}

SCENARIO("NMI integration tests - Full system flow", "[NMI][Integration]") {
  GIVEN("A complete NES system with ROM containing an NMI handler") {
    Bus bus;
    CPUMock cpu{bus};
    PPUMock ppu{bus};

    // Create a ROM with an NMI handler
    // Program layout:
    //   0x8000: Main program (infinite loop: JMP 0x8000)
    //   0x9000: NMI handler (INC $0200, RTI)
    //   0xFFFA: NMI vector pointing to 0x9000
    //   0xFFFC: RESET vector pointing to 0x8000
    std::vector<uint8_t> rom(0x8000, 0xEA); // Fill with NOPs

    // Main program at 0x8000: JMP $8000 (infinite loop)
    rom[0x0000] = 0x4C; // JMP absolute
    rom[0x0001] = 0x00;
    rom[0x0002] = 0x80;

    // NMI handler at 0x9000 (offset 0x1000 in ROM)
    rom[0x1000] = 0xEE; // INC $0200 (increment byte at RAM address 0x0200)
    rom[0x1001] = 0x00;
    rom[0x1002] = 0x02;
    rom[0x1003] = 0x40; // RTI (return from interrupt)

    // Set vectors at end of ROM
    rom[0x7FFA] = 0x00; // NMI vector low byte -> 0x9000
    rom[0x7FFB] = 0x90; // NMI vector high byte
    rom[0x7FFC] = 0x00; // RESET vector low byte -> 0x8000
    rom[0x7FFD] = 0x80; // RESET vector high byte

    auto load_result = bus.LoadIntoProgramRom(rom);
    REQUIRE(load_result.has_value());

    // Initialize CPU
    cpu.Init();
    REQUIRE(cpu.ProgramCounter() == 0x8000);

    // Initialize RAM location we'll increment
    bus.Write(0x0200, 0x00);

    WHEN("PPU generates NMI by entering VBlank") {
      // Enable NMI generation
      ppu.WritePPUCTRL(0b10000000);

      // Tick through one frame to scanline 241 (VBlank)
      ppu.Tick(241 * 341);

      THEN("NMI should be triggered") {
        REQUIRE(ppu.CurrentScanline() == 241);
        REQUIRE(ppu.IsInVblank() == true);

        // CPU should have jumped to NMI handler
        REQUIRE(cpu.ProgramCounter() == 0x9000);

        // Interrupt disable flag should be set
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::InterruptDisable) == true);
      }

      THEN("NMI handler can execute and modify RAM") {
        // Execute the INC $0200 instruction in the NMI handler
        ExecuteOneInstruction(cpu);

        // Check that RAM was incremented
        REQUIRE(bus.Read(0x0200) == 0x01);

        // Execute RTI to return from interrupt
        ExecuteOneInstruction(cpu);

        // PC should be back at main program
        REQUIRE(cpu.ProgramCounter() == 0x8000);
      }
    }

    WHEN("Multiple frames are processed") {
      ppu.WritePPUCTRL(0b10000000);

      // Process 3 complete frames
      for (int frame = 0; frame < 3; ++frame) {
        // Tick to VBlank (scanline 241)
        ppu.Tick(241 * 341);
        REQUIRE(ppu.CurrentScanline() == 241);

        // Execute NMI handler (INC $0200)
        ExecuteOneInstruction(cpu); // INC
        ExecuteOneInstruction(cpu); // RTI

        // Complete the frame (back to scanline 0)
        ppu.Tick(21 * 341);
        REQUIRE(ppu.CurrentScanline() == 0);
      }

      THEN("NMI handler should have executed 3 times") { REQUIRE(bus.Read(0x0200) == 0x03); }
    }

    WHEN("NMI is disabled mid-frame") {
      ppu.WritePPUCTRL(0b10000000); // Enable NMI

      // Tick to scanline 100
      ppu.Tick(100 * 341);

      // Disable NMI
      ppu.WritePPUCTRL(0b00000000);

      // Continue to VBlank
      ppu.Tick(141 * 341);

      THEN("VBlank flag is set but no NMI occurs") {
        REQUIRE(ppu.CurrentScanline() == 241);
        REQUIRE(ppu.IsInVblank() == true);
        REQUIRE(cpu.ProgramCounter() == 0x8000); // Still in main program
        REQUIRE(bus.Read(0x0200) == 0x00);       // Handler didn't run
      }
    }
  }
}

SCENARIO("NMI integration tests - Timing and edge cases", "[NMI][Integration]") {
  GIVEN("A system with NMI handler that takes multiple instructions") {
    Bus bus;
    CPUMock cpu{bus};
    PPUMock ppu{bus};

    // ROM with a more complex NMI handler
    std::vector<uint8_t> rom(0x8000, 0xEA);

    // Main program: infinite loop
    rom[0x0000] = 0x4C;
    rom[0x0001] = 0x00;
    rom[0x0002] = 0x80;

    // NMI handler at 0x9000: Multiple increments then RTI
    rom[0x1000] = 0xEE; // INC $0200
    rom[0x1001] = 0x00;
    rom[0x1002] = 0x02;
    rom[0x1003] = 0xEE; // INC $0201
    rom[0x1004] = 0x01;
    rom[0x1005] = 0x02;
    rom[0x1006] = 0xEE; // INC $0202
    rom[0x1007] = 0x02;
    rom[0x1008] = 0x02;
    rom[0x1009] = 0x40; // RTI

    // Set vectors
    rom[0x7FFA] = 0x00;
    rom[0x7FFB] = 0x90;
    rom[0x7FFC] = 0x00;
    rom[0x7FFD] = 0x80;

    bus.LoadIntoProgramRom(rom);
    cpu.Init();

    // Initialize RAM
    bus.Write(0x0200, 0x00);
    bus.Write(0x0201, 0x00);
    bus.Write(0x0202, 0x00);

    WHEN("NMI occurs and handler executes completely") {
      ppu.WritePPUCTRL(0b10000000);
      ppu.Tick(241 * 341);

      // Execute all instructions in NMI handler
      ExecuteOneInstruction(cpu); // INC $0200
      ExecuteOneInstruction(cpu); // INC $0201
      ExecuteOneInstruction(cpu); // INC $0202
      ExecuteOneInstruction(cpu); // RTI

      THEN("All three RAM locations should be incremented") {
        REQUIRE(bus.Read(0x0200) == 0x01);
        REQUIRE(bus.Read(0x0201) == 0x01);
        REQUIRE(bus.Read(0x0202) == 0x01);
      }

      THEN("CPU should return to main program") {
        REQUIRE(cpu.ProgramCounter() == 0x8000);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::InterruptDisable) == true);
      }
    }

    WHEN("NMI is enabled during VBlank") {
      // Start with NMI disabled
      ppu.WritePPUCTRL(0b00000000);

      // Enter VBlank
      ppu.Tick(241 * 341);
      REQUIRE(ppu.IsInVblank() == true);

      auto pc_before = cpu.ProgramCounter();

      // Enable NMI during VBlank
      ppu.WritePPUCTRL(0b10000000);

      THEN("NMI should trigger immediately") {
        REQUIRE(cpu.ProgramCounter() == 0x9000);
        REQUIRE(cpu.ProgramCounter() != pc_before);
      }
    }
  }
}

SCENARIO("NMI integration tests - RTI behavior", "[NMI][Integration]") {
  GIVEN("A system with NMI handler") {
    Bus bus;
    CPUMock cpu{bus};
    PPUMock ppu{bus};

    std::vector<uint8_t> rom(0x8000, 0xEA);

    // Main program with flags set
    rom[0x0000] = 0x38; // SEC (set carry)
    rom[0x0001] = 0xF8; // SED (set decimal mode)
    rom[0x0002] = 0x4C; // JMP $8000
    rom[0x0003] = 0x00;
    rom[0x0004] = 0x80;

    // NMI handler: just RTI
    rom[0x1000] = 0x40; // RTI

    // Set vectors
    rom[0x7FFA] = 0x00;
    rom[0x7FFB] = 0x90;
    rom[0x7FFC] = 0x00;
    rom[0x7FFD] = 0x80;

    bus.LoadIntoProgramRom(rom);
    cpu.Init();

    WHEN("NMI occurs after setting status flags") {
      // Execute SEC and SED
      ExecuteOneInstruction(cpu); // SEC
      ExecuteOneInstruction(cpu); // SED

      REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
      REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::DecimalMode) == true);

      auto status_before = cpu.StatusFlags();

      // Trigger NMI
      ppu.WritePPUCTRL(0b10000000);
      ppu.Tick(241 * 341);

      // Execute RTI
      ExecuteOneInstruction(cpu);

      THEN("RTI should restore the original status flags") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::DecimalMode) == true);
        // Interrupt disable will be set by NMI, but RTI restores original
        REQUIRE(cpu.StatusFlags() == status_before);
      }
    }
  }
}
