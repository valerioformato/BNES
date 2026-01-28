//
// Created by vformato on 1/28/26.
//

#include "HW/CPU.h"
#include "HW/PPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

class PPUMock : public PPU {
public:
  using PPU::IsInVblank;
  using PPU::PPU;
  using PPU::Tick;
  using PPU::VblankNMIEnabled;
  using PPU::WritePPUCTRL;
};

SCENARIO("PPU NMI generation on VBlank entry", "[PPU][NMI]") {
  GIVEN("A PPU and CPU connected via Bus") {
    Bus bus;
    CPU cpu{bus};
    PPUMock ppu{bus};

    // Setup minimal ROM for NMI vector
    std::vector<uint8_t> rom(0x8000, 0x00);
    rom[0x7FFA] = 0x34; // NMI vector low byte
    rom[0x7FFB] = 0x12; // NMI vector high byte -> 0x1234
    auto load_result = bus.LoadIntoProgramRom(rom);
    REQUIRE(load_result.has_value());

    WHEN("VBlank is entered at scanline 241 with NMI enabled") {
      ppu.WritePPUCTRL(0b10000000); // Enable VBlank NMI
      REQUIRE(ppu.VblankNMIEnabled() == true);

      // Tick to scanline 240 (right before VBlank)
      ppu.Tick(240 * 341);
      REQUIRE(ppu.CurrentScanline() == 240);
      REQUIRE(ppu.IsInVblank() == false);

      auto original_pc = cpu.ProgramCounter();
      auto original_sp = cpu.StackPointer();

      // Tick to scanline 241 (VBlank start)
      ppu.Tick(341);

      THEN("NMI should be triggered and CPU state should reflect NMI processing") {
        REQUIRE(ppu.CurrentScanline() == 241);
        REQUIRE(ppu.IsInVblank() == true);
        REQUIRE(cpu.ProgramCounter() == 0x1234);        // Jumped to NMI vector
        REQUIRE(cpu.StackPointer() == original_sp - 3); // Stack pushed 3 bytes
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::InterruptDisable) == true);
      }
    }

    WHEN("VBlank is entered at scanline 241 with NMI disabled") {
      ppu.WritePPUCTRL(0b00000000); // Disable VBlank NMI
      REQUIRE(ppu.VblankNMIEnabled() == false);

      auto original_pc = cpu.ProgramCounter();
      auto original_sp = cpu.StackPointer();

      // Tick to scanline 241
      ppu.Tick(241 * 341);

      THEN("NMI should not be triggered") {
        REQUIRE(ppu.CurrentScanline() == 241);
        REQUIRE(ppu.IsInVblank() == true);
        REQUIRE(cpu.ProgramCounter() == original_pc); // PC unchanged
        REQUIRE(cpu.StackPointer() == original_sp);   // Stack unchanged
      }
    }

    WHEN("VBlank flag is cleared at scanline 0") {
      ppu.WritePPUCTRL(0b10000000); // Enable VBlank NMI

      // Enter VBlank
      ppu.Tick(241 * 341);
      REQUIRE(ppu.IsInVblank() == true);

      // Complete the frame (scanline 261) and go to scanline 0
      ppu.Tick(21 * 341);

      THEN("VBlank flag should be cleared") {
        REQUIRE(ppu.CurrentScanline() == 0);
        REQUIRE(ppu.IsInVblank() == false);
      }
    }

    WHEN("Multiple frames are rendered") {
      ppu.WritePPUCTRL(0b10000000); // Enable VBlank NMI

      // First frame
      ppu.Tick(241 * 341);
      auto sp_after_first_nmi = cpu.StackPointer();

      // Complete first frame and start second
      ppu.Tick(21 * 341); // Scanlines 241-261
      REQUIRE(ppu.CurrentScanline() == 0);

      // Second frame to VBlank
      ppu.Tick(241 * 341);
      auto sp_after_second_nmi = cpu.StackPointer();

      THEN("NMI should trigger once per frame") { REQUIRE(sp_after_second_nmi == sp_after_first_nmi - 3); }
    }

    WHEN("Scanlines before 241 are processed") {
      ppu.WritePPUCTRL(0b10000000); // Enable VBlank NMI

      auto original_pc = cpu.ProgramCounter();
      auto original_sp = cpu.StackPointer();

      // Tick through visible scanlines (1-239) and post-render scanline (240)
      // VBlank flag should remain false throughout all these scanlines
      for (int scanline = 1; scanline <= 240; ++scanline) {
        ppu.Tick(341);
        REQUIRE(ppu.CurrentScanline() == scanline);
        REQUIRE(ppu.IsInVblank() == false);
      }

      THEN("No NMI should be triggered before scanline 241") {
        REQUIRE(ppu.CurrentScanline() == 240);
        REQUIRE(cpu.ProgramCounter() == original_pc);
        REQUIRE(cpu.StackPointer() == original_sp);
      }
    }
  }
}

SCENARIO("PPU NMI generation on PPUCTRL write", "[PPU][NMI]") {
  GIVEN("A PPU and CPU connected via Bus in VBlank") {
    Bus bus;
    CPU cpu{bus};
    PPUMock ppu{bus};

    // Setup minimal ROM for NMI vector
    std::vector<uint8_t> rom(0x8000, 0x00);
    rom[0x7FFA] = 0x78; // NMI vector low byte
    rom[0x7FFB] = 0x56; // NMI vector high byte -> 0x5678
    auto load_result = bus.LoadIntoProgramRom(rom);
    REQUIRE(load_result.has_value());

    WHEN("NMI is enabled during VBlank (0 to 1 transition)") {
      ppu.WritePPUCTRL(0b00000000); // Disable VBlank NMI
      REQUIRE(ppu.VblankNMIEnabled() == false);

      // Enter VBlank with NMI disabled
      ppu.Tick(241 * 341);
      REQUIRE(ppu.IsInVblank() == true);

      auto original_pc = cpu.ProgramCounter();
      auto original_sp = cpu.StackPointer();

      // Enable NMI during VBlank
      ppu.WritePPUCTRL(0b10000000);

      THEN("NMI should be triggered immediately") {
        REQUIRE(ppu.VblankNMIEnabled() == true);
        REQUIRE(cpu.ProgramCounter() == 0x5678);
        REQUIRE(cpu.StackPointer() == original_sp - 3);
      }
    }

    WHEN("NMI remains enabled during VBlank (1 to 1 transition)") {
      ppu.WritePPUCTRL(0b10000000); // Enable VBlank NMI

      // Enter VBlank
      ppu.Tick(241 * 341);
      auto sp_after_first_nmi = cpu.StackPointer();

      // Write to PPUCTRL again with NMI still enabled
      ppu.WritePPUCTRL(0b10000011); // NMI still enabled, change other bits

      THEN("No additional NMI should be triggered") {
        REQUIRE(ppu.VblankNMIEnabled() == true);
        REQUIRE(cpu.StackPointer() == sp_after_first_nmi); // No change
      }
    }

    WHEN("NMI is disabled during VBlank (1 to 0 transition)") {
      ppu.WritePPUCTRL(0b10000000); // Enable VBlank NMI

      // Enter VBlank
      ppu.Tick(241 * 341);
      auto sp_after_nmi = cpu.StackPointer();

      // Disable NMI during VBlank
      ppu.WritePPUCTRL(0b00000000);

      THEN("No additional NMI should be triggered") {
        REQUIRE(ppu.VblankNMIEnabled() == false);
        REQUIRE(cpu.StackPointer() == sp_after_nmi); // No change
      }
    }

    WHEN("NMI is enabled outside of VBlank") {
      ppu.WritePPUCTRL(0b00000000); // Disable VBlank NMI

      // Tick to scanline 100 (not in VBlank)
      ppu.Tick(100 * 341);
      REQUIRE(ppu.IsInVblank() == false);

      auto original_pc = cpu.ProgramCounter();
      auto original_sp = cpu.StackPointer();

      // Enable NMI outside VBlank
      ppu.WritePPUCTRL(0b10000000);

      THEN("NMI should not be triggered") {
        REQUIRE(ppu.VblankNMIEnabled() == true);
        REQUIRE(cpu.ProgramCounter() == original_pc);
        REQUIRE(cpu.StackPointer() == original_sp);
      }
    }

    WHEN("NMI is toggled multiple times outside VBlank") {
      auto original_pc = cpu.ProgramCounter();
      auto original_sp = cpu.StackPointer();

      ppu.Tick(100 * 341);
      REQUIRE(ppu.IsInVblank() == false);

      ppu.WritePPUCTRL(0b10000000); // Enable
      ppu.WritePPUCTRL(0b00000000); // Disable
      ppu.WritePPUCTRL(0b10000000); // Enable again
      ppu.WritePPUCTRL(0b00000000); // Disable again

      THEN("No NMI should be triggered outside VBlank") {
        REQUIRE(cpu.ProgramCounter() == original_pc);
        REQUIRE(cpu.StackPointer() == original_sp);
      }
    }

    WHEN("NMI is enabled right at the start of VBlank") {
      ppu.WritePPUCTRL(0b00000000); // Start with NMI disabled

      // Tick to exactly scanline 241, cycle 0
      ppu.Tick(241 * 341);
      REQUIRE(ppu.CurrentScanline() == 241);

      auto original_sp = cpu.StackPointer();

      // Enable NMI at the very start of VBlank
      ppu.WritePPUCTRL(0b10000000);

      THEN("NMI should be triggered immediately") { REQUIRE(cpu.StackPointer() == original_sp - 3); }
    }

    WHEN("NMI is enabled near the end of VBlank") {
      ppu.WritePPUCTRL(0b00000000);

      // Enter VBlank without NMI
      ppu.Tick(241 * 341);
      REQUIRE(ppu.IsInVblank() == true);

      // Tick through most of VBlank (scanlines 241-260)
      ppu.Tick(19 * 341);
      REQUIRE(ppu.CurrentScanline() == 260);

      auto original_sp = cpu.StackPointer();

      // Enable NMI near end of VBlank
      ppu.WritePPUCTRL(0b10000000);

      THEN("NMI should still be triggered") { REQUIRE(cpu.StackPointer() == original_sp - 3); }
    }
  }
}

SCENARIO("PPU NMI edge cases", "[PPU][NMI]") {
  GIVEN("A PPU and CPU connected via Bus") {
    Bus bus;
    CPU cpu{bus};
    PPUMock ppu{bus};

    std::vector<uint8_t> rom(0x8000, 0x00);
    rom[0x7FFA] = 0xAB;
    rom[0x7FFB] = 0xCD; // NMI vector -> 0xCDAB
    auto load_result = bus.LoadIntoProgramRom(rom);
    REQUIRE(load_result.has_value());

    WHEN("NMI is disabled just before entering VBlank") {
      ppu.WritePPUCTRL(0b10000000); // Enable NMI

      // Tick to scanline 240 (just before VBlank)
      ppu.Tick(240 * 341);
      REQUIRE(ppu.CurrentScanline() == 240);

      // Disable NMI before VBlank
      ppu.WritePPUCTRL(0b00000000);

      auto original_pc = cpu.ProgramCounter();
      auto original_sp = cpu.StackPointer();

      // Enter VBlank
      ppu.Tick(341);

      THEN("No NMI should be triggered") {
        REQUIRE(ppu.CurrentScanline() == 241);
        REQUIRE(ppu.IsInVblank() == true);
        REQUIRE(cpu.ProgramCounter() == original_pc);
        REQUIRE(cpu.StackPointer() == original_sp);
      }
    }

    WHEN("NMI is enabled, disabled, then re-enabled during VBlank") {
      ppu.WritePPUCTRL(0b00000000);

      // Enter VBlank
      ppu.Tick(241 * 341);
      REQUIRE(ppu.IsInVblank() == true);

      // Enable NMI (should trigger)
      ppu.WritePPUCTRL(0b10000000);
      auto sp_after_first = cpu.StackPointer();

      // Disable NMI
      ppu.WritePPUCTRL(0b00000000);
      REQUIRE(cpu.StackPointer() == sp_after_first);

      // Re-enable NMI (should trigger again due to 0->1 transition)
      ppu.WritePPUCTRL(0b10000000);

      THEN("NMI should trigger again on second 0->1 transition") { REQUIRE(cpu.StackPointer() == sp_after_first - 3); }
    }

    WHEN("Frame timing wraps from scanline 261 to 0") {
      ppu.WritePPUCTRL(0b10000000);

      // Go through entire frame
      ppu.Tick(241 * 341); // Scanline 241 (VBlank starts, NMI triggered)
      auto sp_after_nmi = cpu.StackPointer();
      REQUIRE(ppu.IsInVblank() == true);

      // Complete the VBlank period
      ppu.Tick(21 * 341); // Scanlines 242-261 and wrap to 0

      THEN("VBlank should be cleared and scanline should wrap") {
        REQUIRE(ppu.CurrentScanline() == 0);
        REQUIRE(ppu.IsInVblank() == false);
        REQUIRE(cpu.StackPointer() == sp_after_nmi); // No additional NMI
      }
    }

    WHEN("Writing to PPUCTRL with only non-NMI bits during VBlank") {
      ppu.WritePPUCTRL(0b00001111); // NMI disabled, other bits set

      // Enter VBlank
      ppu.Tick(241 * 341);
      REQUIRE(ppu.IsInVblank() == true);

      auto original_pc = cpu.ProgramCounter();
      auto original_sp = cpu.StackPointer();

      // Change other bits but keep NMI disabled
      ppu.WritePPUCTRL(0b01111111); // NMI still disabled (bit 7 = 0)

      THEN("No NMI should be triggered") {
        REQUIRE(ppu.VblankNMIEnabled() == false);
        REQUIRE(cpu.ProgramCounter() == original_pc);
        REQUIRE(cpu.StackPointer() == original_sp);
      }
    }
  }
}
