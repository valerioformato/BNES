//
// Created by Valerio Formato on 23-Jan-26.
//
#include "HW/PPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

class PPUMock : public PPU {
public:
  using PPU::AddressRegister;
  using PPU::BackgroundPatternTableAddress;
  using PPU::BaseNametableAddress;
  using PPU::PPU;
  using PPU::PPUMasterSlaveSelect;
  using PPU::ReadPPUDATA;
  using PPU::SpritePatternTableAddress;
  using PPU::SpriteSize;
  using PPU::VblankNMIEnable;
  using PPU::VRAMAddressIncrement;
  using PPU::WritePPUADDR;
  using PPU::WritePPUCTRL;
  using PPU::WritePPUDATA;
  void WriteToVRAM(Addr addr, uint8_t value) { m_vram[addr] = value; }
  void WriteToPalette(uint8_t index, uint8_t value) { m_palette_table[index] = value; }
  uint8_t ReadFromVRAM(Addr addr) const { return m_vram[addr]; }
  uint8_t ReadFromPalette(uint8_t index) const { return m_palette_table[index]; }
};

SCENARIO("NES PPU initialization", "[PPU]") {
  GIVEN("a new PPU instance") {

    Bus bus;

    PPUMock ppu{bus};

    WHEN("the PPU is initialized") {
      THEN("the PPU registers should be in their default state") {
        auto registers = ppu.InternalRegisters();
        REQUIRE(registers[PPU::Register::V] == 0x0000);
        REQUIRE(registers[PPU::Register::T] == 0x0000);
        REQUIRE(registers[PPU::Register::X] == 0x0000);
        REQUIRE(registers[PPU::Register::W] == 0x0000);

        REQUIRE(ppu.AddressRegister() == 0x0000);
      }
    }
  }
}

SCENARIO("MMIO registers tests", "[PPU]") {
  GIVEN("a PPU instance") {
    Bus bus;
    PPUMock ppu{bus};

    WHEN("writing to PPUADDR for the first time") {
      ppu.WritePPUADDR(0x12);

      THEN("the high byte should be set with bits 0-5 only") {
        REQUIRE(ppu.AddressRegister() == 0x1200);

        auto internal_registers = ppu.InternalRegisters();
        REQUIRE(internal_registers[PPU::Register::T] == 0x1200);
        REQUIRE(internal_registers[PPU::Register::W] == 1);
      }
    }

    WHEN("writing to PPUADDR twice") {
      ppu.WritePPUADDR(0x3F);
      ppu.WritePPUADDR(0x20);

      THEN("the full 14-bit address should be set") {
        REQUIRE(ppu.AddressRegister() == 0x3F20);

        auto internal_registers = ppu.InternalRegisters();
        REQUIRE(internal_registers[PPU::Register::T] == 0x3F20);
        REQUIRE(internal_registers[PPU::Register::W] == 0);
      }
    }

    WHEN("writing to PPUADDR with high bits set in first write") {
      ppu.WritePPUADDR(0xFF);

      THEN("the high bits should be masked to 6 bits") {
        REQUIRE(ppu.AddressRegister() == 0x3F00);

        auto internal_registers = ppu.InternalRegisters();
        REQUIRE(internal_registers[PPU::Register::T] == 0x3F00);
        REQUIRE(internal_registers[PPU::Register::W] == 1);
      }
    }

    WHEN("writing to PPUADDR four times") {
      ppu.WritePPUADDR(0x20);
      ppu.WritePPUADDR(0x00);
      ppu.WritePPUADDR(0x21);
      ppu.WritePPUADDR(0x34);

      THEN("the address should be updated with the third and fourth writes") {
        REQUIRE(ppu.AddressRegister() == 0x2134);

        auto internal_registers = ppu.InternalRegisters();
        REQUIRE(internal_registers[PPU::Register::T] == 0x2134);
        REQUIRE(internal_registers[PPU::Register::W] == 0);
      }
    }
  }
}

SCENARIO("PPUCTRL register tests", "[PPU]") {
  GIVEN("a PPU instance") {
    Bus bus;
    PPUMock ppu{bus};

    WHEN("writing 0x00 to PPUCTRL") {
      ppu.WritePPUCTRL(0x00);

      THEN("all control flags should be disabled") {
        REQUIRE(ppu.BaseNametableAddress() == 0);
        REQUIRE(ppu.VRAMAddressIncrement() == false);
        REQUIRE(ppu.SpritePatternTableAddress() == false);
        REQUIRE(ppu.BackgroundPatternTableAddress() == false);
        REQUIRE(ppu.SpriteSize() == false);
        REQUIRE(ppu.PPUMasterSlaveSelect() == false);
        REQUIRE(ppu.VblankNMIEnable() == false);
      }
    }

    WHEN("writing 0xFF to PPUCTRL") {
      ppu.WritePPUCTRL(0xFF);

      THEN("all control flags should be enabled") {
        REQUIRE(ppu.BaseNametableAddress() == 0b00000011);
        REQUIRE(ppu.VRAMAddressIncrement() == true);
        REQUIRE(ppu.SpritePatternTableAddress() == true);
        REQUIRE(ppu.BackgroundPatternTableAddress() == true);
        REQUIRE(ppu.SpriteSize() == true);
        REQUIRE(ppu.PPUMasterSlaveSelect() == true);
        REQUIRE(ppu.VblankNMIEnable() == true);
      }
    }

    WHEN("writing with VRAM address increment bit clear (bit 2 = 0)") {
      ppu.WritePPUCTRL(0b00000000);

      THEN("the VRAM address increment should be 1") { REQUIRE(ppu.VRAMAddressIncrement() == false); }
    }

    WHEN("writing with VRAM address increment bit set (bit 2 = 1)") {
      ppu.WritePPUCTRL(0b00000100);

      THEN("the VRAM address increment should be 32") { REQUIRE(ppu.VRAMAddressIncrement() == true); }
    }

    WHEN("writing to set base nametable address") {
      ppu.WritePPUCTRL(0b00000010);

      THEN("the base nametable address bits should be set correctly") {
        REQUIRE(ppu.BaseNametableAddress() == 0b00000010);
      }
    }

    WHEN("writing to enable VBlank NMI") {
      ppu.WritePPUCTRL(0b10000000);

      THEN("the VBlank NMI flag should be enabled") { REQUIRE(ppu.VblankNMIEnable() == true); }
    }

    WHEN("writing to set sprite pattern table address") {
      ppu.WritePPUCTRL(0b00001000);

      THEN("the sprite pattern table address flag should be set") { REQUIRE(ppu.SpritePatternTableAddress() == true); }
    }

    WHEN("writing to set background pattern table address") {
      ppu.WritePPUCTRL(0b00010000);

      THEN("the background pattern table address flag should be set") {
        REQUIRE(ppu.BackgroundPatternTableAddress() == true);
      }
    }

    WHEN("writing to set sprite size") {
      ppu.WritePPUCTRL(0b00100000);

      THEN("the sprite size flag should be set") { REQUIRE(ppu.SpriteSize() == true); }
    }

    WHEN("writing to set PPU master/slave select") {
      ppu.WritePPUCTRL(0b01000000);

      THEN("the PPU master/slave select flag should be set") { REQUIRE(ppu.PPUMasterSlaveSelect() == true); }
    }

    WHEN("writing multiple times to PPUCTRL") {
      ppu.WritePPUCTRL(0b10101010);
      ppu.WritePPUCTRL(0b01010101);

      THEN("the register should hold the last written value") {
        REQUIRE(ppu.BaseNametableAddress() == 0b00000001);
        REQUIRE(ppu.VRAMAddressIncrement() == true);
        REQUIRE(ppu.SpritePatternTableAddress() == false);
        REQUIRE(ppu.BackgroundPatternTableAddress() == true);
        REQUIRE(ppu.SpriteSize() == false);
        REQUIRE(ppu.PPUMasterSlaveSelect() == true);
        REQUIRE(ppu.VblankNMIEnable() == false);
      }
    }
  }
}

SCENARIO("PPUDATA register read tests", "[PPU]") {
  GIVEN("a PPU instance with test data") {
    Bus bus;
    PPUMock ppu{bus};

    WHEN("reading from VRAM") {
      // Set up test data in VRAM
      ppu.WriteToVRAM(0x200, 0xAB);
      ppu.WriteToVRAM(0x201, 0xCD);

      // Set address to VRAM region (0x2000 + offset)
      // Address 0x2200 maps to VRAM index 0x2200 - 0x2000 = 0x200
      ppu.WritePPUADDR(0x22);
      ppu.WritePPUADDR(0x00);

      THEN("the first read should return the buffered value (0)") {
        auto first_read = ppu.ReadPPUDATA();  // First read buffers the value, returns 0
        REQUIRE(first_read == 0x00);  // Initial buffer is 0
        
        auto second_read = ppu.ReadPPUDATA();  // Second read returns the buffered value from first read
        REQUIRE(second_read == 0xAB);
      }

      THEN("subsequent reads should return the previous buffered values") {
        auto first_read = ppu.ReadPPUDATA();  // Returns 0, buffers 0xAB from address 0x2200
        REQUIRE(first_read == 0x00);

        auto second_read = ppu.ReadPPUDATA();  // Returns 0xAB, buffers 0xCD from address 0x2201
        REQUIRE(second_read == 0xAB);
        
        auto third_read = ppu.ReadPPUDATA();  // Returns 0xCD, buffers 0 from address 0x2202
        REQUIRE(third_read == 0xCD);
        
        auto fourth_read = ppu.ReadPPUDATA();  // Returns 0
        REQUIRE(fourth_read == 0x00);
      }
    }

    WHEN("reading from palette table") {
      // Set up test data in palette table
      ppu.WriteToPalette(0, 0x12);
      ppu.WriteToPalette(1, 0x34);

      // Set address to palette table region (0x3F00+)
      ppu.WritePPUADDR(0x3F);
      ppu.WritePPUADDR(0x01);

      THEN("palette reads return immediately without buffering") {
        auto first_read = ppu.ReadPPUDATA();  // Palette reads return immediately
        REQUIRE(first_read == 0x34);
        
        auto second_read = ppu.ReadPPUDATA();  // Next palette value (uninitialized)
        REQUIRE(second_read == 0x00);
      }

      THEN("palette reads return data immediately") {
        auto first_read = ppu.ReadPPUDATA();  // Returns palette[1] = 0x34 immediately
        REQUIRE(first_read == 0x34);
        
        auto second_read = ppu.ReadPPUDATA();  // Returns palette[2] (uninitialized) = 0x00
        REQUIRE(second_read == 0x00);
        
        auto third_read = ppu.ReadPPUDATA();  // Returns palette[3] (uninitialized) = 0x00
        REQUIRE(third_read == 0x00);
      }
    }

    WHEN("reading from multiple VRAM addresses sequentially") {
      // Set up test data
      ppu.WriteToVRAM(0x200, 0xAA);
      ppu.WriteToVRAM(0x201, 0xBB);
      ppu.WriteToVRAM(0x202, 0xCC);

      ppu.WritePPUADDR(0x22);
      ppu.WritePPUADDR(0x00);

      THEN("sequential reads should return buffered values in order") {
        auto read1 = ppu.ReadPPUDATA();  // Returns 0, buffers 0xAA
        REQUIRE(read1 == 0x00);

        auto read2 = ppu.ReadPPUDATA();  // Returns 0xAA, buffers 0xBB
        REQUIRE(read2 == 0xAA);

        auto read3 = ppu.ReadPPUDATA();  // Returns 0xBB, buffers 0xCC
        REQUIRE(read3 == 0xBB);
        
        auto read4 = ppu.ReadPPUDATA();  // Returns 0xCC, buffers 0
        REQUIRE(read4 == 0xCC);
        
        auto read5 = ppu.ReadPPUDATA();  // Returns 0
        REQUIRE(read5 == 0x00);
      }
    }

    WHEN("reading from address 0x2000 (start of VRAM)") {
      ppu.WriteToVRAM(0, 0x55);

      ppu.WritePPUADDR(0x20);
      ppu.WritePPUADDR(0x00);

      THEN("the read should access VRAM correctly") {
        auto first_read = ppu.ReadPPUDATA();  // Returns 0, buffers 0x55
        REQUIRE(first_read == 0x00);
        
        auto second_read = ppu.ReadPPUDATA();  // Returns 0x55
        REQUIRE(second_read == 0x55);
      }
    }

    WHEN("reading from address 0x2FFF (end of VRAM)") {
      ppu.WriteToVRAM(0x7FE, 0x99);

      ppu.WritePPUADDR(0x2F);
      ppu.WritePPUADDR(0xFE);  // Address 0x2FFE

      THEN("the read should access VRAM correctly") {
        auto first_read = ppu.ReadPPUDATA();  // Returns 0, buffers 0x99
        REQUIRE(first_read == 0x00);
        
        auto second_read = ppu.ReadPPUDATA();  // Returns 0x99
        REQUIRE(second_read == 0x99);
      }
    }

    WHEN("reading from palette table at 0x3F00") {
      ppu.WriteToPalette(0, 0x77);

      ppu.WritePPUADDR(0x3F);
      ppu.WritePPUADDR(0x00);

      THEN("palette reads return immediately without buffering") {
        auto first_read = ppu.ReadPPUDATA();  // Palette reads return immediately
        REQUIRE(first_read == 0x77);
        
        ppu.WriteToPalette(1, 0x88);
        auto second_read = ppu.ReadPPUDATA();  // Next palette value
        REQUIRE(second_read == 0x88);
      }
    }

    WHEN("reading from palette table at 0x3F1F (within addressable palette)") {
      ppu.WriteToPalette(0x1F, 0x88);
      
      ppu.WritePPUADDR(0x3F);
      ppu.WritePPUADDR(0x1F);

      THEN("palette reads return immediately") {
        auto first_read = ppu.ReadPPUDATA();  // Palette reads return immediately
        REQUIRE(first_read == 0x88);
        
        ppu.WriteToPalette(0, 0x99);  // Wraps to 0x3F00 after increment
        auto second_read = ppu.ReadPPUDATA();  // Reads from 0x3F20, wraps to 0x3F00
        REQUIRE(second_read == 0x99);
      }
    }
  }
}

SCENARIO("PPUDATA register write tests", "[PPU]") {
  GIVEN("a PPU instance") {
    Bus bus;
    PPUMock ppu{bus};

    WHEN("writing to VRAM region (0x2000-0x2FFF)") {
      ppu.WritePPUADDR(0x20);
      ppu.WritePPUADDR(0x50);
      ppu.WritePPUDATA(0xAB);

      THEN("the value should be written to VRAM at the correct mirrored address") {
        // Address 0x2050 maps to VRAM index 0x50
        REQUIRE(ppu.ReadFromVRAM(0x50) == 0xAB);
      }
    }

    WHEN("writing to VRAM at address 0x2000") {
      ppu.WritePPUADDR(0x20);
      ppu.WritePPUADDR(0x00);
      ppu.WritePPUDATA(0x12);

      THEN("the value should be written to VRAM index 0") {
        REQUIRE(ppu.ReadFromVRAM(0x00) == 0x12);
      }
    }

    WHEN("writing to VRAM at address 0x2FFF") {
      ppu.WritePPUADDR(0x2F);
      ppu.WritePPUADDR(0xFF);
      ppu.WritePPUDATA(0x99);

      THEN("the value should be written to VRAM at the correct mirrored address") {
        // Address 0x2FFF - 0x2000 = 0x7FF
        REQUIRE(ppu.ReadFromVRAM(0x7FF) == 0x99);
      }
    }

    WHEN("writing to palette table region (0x3F00-0x3FFF)") {
      ppu.WritePPUADDR(0x3F);
      ppu.WritePPUADDR(0x00);
      ppu.WritePPUDATA(0x25);

      THEN("the value should be written to the palette table at index 0") {
        REQUIRE(ppu.ReadFromPalette(0) == 0x25);
      }
    }

    WHEN("writing to palette table at address 0x3F10") {
      ppu.WritePPUADDR(0x3F);
      ppu.WritePPUADDR(0x10);
      ppu.WritePPUDATA(0x38);

      THEN("the value should be written to palette index 0x10") {
        REQUIRE(ppu.ReadFromPalette(0x10) == 0x38);
      }
    }

    WHEN("writing to palette table at address 0x3F1F") {
      ppu.WritePPUADDR(0x3F);
      ppu.WritePPUADDR(0x1F);
      ppu.WritePPUDATA(0x3D);

      THEN("the value should be written to palette index 0x1F") {
        REQUIRE(ppu.ReadFromPalette(0x1F) == 0x3D);
      }
    }

    WHEN("writing to palette table with mirroring (0x3F20-0x3FFF)") {
      ppu.WritePPUADDR(0x3F);
      ppu.WritePPUADDR(0x25);
      ppu.WritePPUDATA(0x15);

      THEN("the value should be written with palette table wrapping") {
        // 0x3F25 - 0x3F00 = 0x25, 0x25 % 0x20 = 0x05
        REQUIRE(ppu.ReadFromPalette(0x05) == 0x15);
      }
    }

    WHEN("writing to palette table at 0x3FFF") {
      ppu.WritePPUADDR(0x3F);
      ppu.WritePPUADDR(0xFF);
      ppu.WritePPUDATA(0x3A);

      THEN("the value should be written with palette table wrapping") {
        // 0x3FFF - 0x3F00 = 0xFF, 0xFF % 0x20 = 0x1F
        REQUIRE(ppu.ReadFromPalette(0x1F) == 0x3A);
      }
    }

    WHEN("writing multiple values to VRAM sequentially") {
      ppu.WritePPUADDR(0x21);
      ppu.WritePPUADDR(0x00);
      ppu.WritePPUDATA(0x11);
      ppu.WritePPUDATA(0x22);
      ppu.WritePPUDATA(0x33);

      THEN("each value should be written with address auto-increment") {
        // Default increment is 1
        REQUIRE(ppu.ReadFromVRAM(0x100) == 0x11);
        REQUIRE(ppu.ReadFromVRAM(0x101) == 0x22);
        REQUIRE(ppu.ReadFromVRAM(0x102) == 0x33);
      }
    }

    WHEN("writing multiple values to palette table sequentially") {
      ppu.WritePPUADDR(0x3F);
      ppu.WritePPUADDR(0x08);
      ppu.WritePPUDATA(0xAA);
      ppu.WritePPUDATA(0xBB);
      ppu.WritePPUDATA(0xCC);

      THEN("each value should be written with address auto-increment") {
        REQUIRE(ppu.ReadFromPalette(0x08) == 0xAA);
        REQUIRE(ppu.ReadFromPalette(0x09) == 0xBB);
        REQUIRE(ppu.ReadFromPalette(0x0A) == 0xCC);
      }
    }

    WHEN("writing with VRAM address increment set to 32") {
      ppu.WritePPUCTRL(0b00000100);  // Set VRAM increment to 32
      ppu.WritePPUADDR(0x20);
      ppu.WritePPUADDR(0x00);
      ppu.WritePPUDATA(0xF1);
      ppu.WritePPUDATA(0xF2);

      THEN("the address should increment by 32 after each write") {
        REQUIRE(ppu.ReadFromVRAM(0x00) == 0xF1);
        REQUIRE(ppu.ReadFromVRAM(0x20) == 0xF2);
      }
    }

    WHEN("writing to CHR ROM region (< 0x2000)") {
      ppu.WritePPUADDR(0x10);
      ppu.WritePPUADDR(0x00);

      THEN("writing to PPUDATA should throw an exception") {
        REQUIRE_THROWS_AS(ppu.WritePPUDATA(0xFF), std::runtime_error);
      }
    }

    WHEN("writing to address 0x0000 (CHR ROM)") {
      ppu.WritePPUADDR(0x00);
      ppu.WritePPUADDR(0x00);

      THEN("writing to PPUDATA should throw an exception") {
        REQUIRE_THROWS_AS(ppu.WritePPUDATA(0xFF), std::runtime_error);
      }
    }

    WHEN("writing to address 0x1FFF (last CHR ROM address)") {
      ppu.WritePPUADDR(0x1F);
      ppu.WritePPUADDR(0xFF);

      THEN("writing to PPUDATA should throw an exception") {
        REQUIRE_THROWS_AS(ppu.WritePPUDATA(0xFF), std::runtime_error);
      }
    }

    WHEN("writing a sequence of values across VRAM and palette regions") {
      // Write to VRAM
      ppu.WritePPUADDR(0x23);
      ppu.WritePPUADDR(0xC0);
      ppu.WritePPUDATA(0x55);
      
      // Write to palette
      ppu.WritePPUADDR(0x3F);
      ppu.WritePPUADDR(0x00);
      ppu.WritePPUDATA(0x0F);
      
      // Write to VRAM again
      ppu.WritePPUADDR(0x24);
      ppu.WritePPUADDR(0x00);
      ppu.WritePPUDATA(0xAA);

      THEN("all values should be written to their respective memory regions") {
        REQUIRE(ppu.ReadFromVRAM(0x3C0) == 0x55);
        REQUIRE(ppu.ReadFromPalette(0x00) == 0x0F);
        REQUIRE(ppu.ReadFromVRAM(0x400) == 0xAA);
      }
    }

    WHEN("writing to VRAM with address wrapping") {
      ppu.WritePPUADDR(0x2F);
      ppu.WritePPUADDR(0xFE);
      ppu.WritePPUDATA(0xDD);
      ppu.WritePPUDATA(0xEE);  // Should wrap around

      THEN("writes should handle address wrapping correctly") {
        REQUIRE(ppu.ReadFromVRAM(0x7FE) == 0xDD);
        REQUIRE(ppu.ReadFromVRAM(0x7FF) == 0xEE);
      }
    }
  }
}
