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
  void WriteToVRAM(Addr addr, uint8_t value) { m_vram[addr] = value; }
  void WriteToPalette(uint8_t index, uint8_t value) { m_palette_table[index] = value; }
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
      ppu.WriteToVRAM(0x100, 0xAB);
      ppu.WriteToVRAM(0x101, 0xCD);

      // Set address to VRAM region (0x2000 + offset)
      // Address 0x2200 maps to VRAM index (0x2200 - 0x2000) / 2 = 0x100
      ppu.WritePPUADDR(0x22);
      ppu.WritePPUADDR(0x00);

      THEN("the first read should return the buffered value (0)") {
        auto first_read = ppu.ReadPPUDATA();  // First read buffers the value, returns 0
        REQUIRE(first_read.has_value());
        REQUIRE(first_read.value() == 0x00);  // Initial buffer is 0
        
        auto second_read = ppu.ReadPPUDATA();  // Second read returns the buffered value from first read
        REQUIRE(second_read.has_value());
        REQUIRE(second_read.value() == 0xAB);
      }

      THEN("subsequent reads should return the previous buffered values") {
        auto first_read = ppu.ReadPPUDATA();  // Returns 0, buffers 0xAB from address 0x2200
        REQUIRE(first_read.has_value());
        REQUIRE(first_read.value() == 0x00);

        auto second_read = ppu.ReadPPUDATA();  // Returns 0xAB, buffers 0xAB from address 0x2202 (still 0xAB due to /2)
        REQUIRE(second_read.has_value());
        REQUIRE(second_read.value() == 0xAB);
        
        auto third_read = ppu.ReadPPUDATA();  // Returns 0xAB, buffers 0xCD from address 0x2204 (maps to 0x101)
        REQUIRE(third_read.has_value());
        REQUIRE(third_read.value() == 0xAB);
        
        auto fourth_read = ppu.ReadPPUDATA();  // Returns 0xCD
        REQUIRE(fourth_read.has_value());
        REQUIRE(fourth_read.value() == 0xCD);
      }
    }

    WHEN("reading from palette table") {
      // Set up test data in palette table
      ppu.WriteToPalette(0, 0x12);
      ppu.WriteToPalette(1, 0x34);

      // Set address to palette table region (0x3F00+)
      ppu.WritePPUADDR(0x3F);
      ppu.WritePPUADDR(0x01);

      THEN("the first read should return the buffered value") {
        auto first_read = ppu.ReadPPUDATA();  // Returns 0, buffers palette[1]
        REQUIRE(first_read.has_value());
        REQUIRE(first_read.value() == 0x00);
        
        auto second_read = ppu.ReadPPUDATA();  // Returns palette[1] = 0x34
        REQUIRE(second_read.has_value());
        REQUIRE(second_read.value() == 0x34);
      }

      THEN("the second read should return the data from palette table") {
        auto first_read = ppu.ReadPPUDATA();  // Returns 0, buffers palette[1] = 0x34
        REQUIRE(first_read.has_value());
        REQUIRE(first_read.value() == 0x00);
        
        auto second_read = ppu.ReadPPUDATA();  // Returns 0x34, buffers palette[2]
        REQUIRE(second_read.has_value());
        REQUIRE(second_read.value() == 0x34);
        
        auto third_read = ppu.ReadPPUDATA();  // Returns palette[2]
        REQUIRE(third_read.has_value());
      }
    }

    WHEN("reading from multiple VRAM addresses sequentially") {
      // Set up test data
      ppu.WriteToVRAM(0x100, 0xAA);
      ppu.WriteToVRAM(0x101, 0xBB);
      ppu.WriteToVRAM(0x102, 0xCC);

      ppu.WritePPUADDR(0x22);
      ppu.WritePPUADDR(0x00);

      THEN("sequential reads should return buffered values in order") {
        auto read1 = ppu.ReadPPUDATA();  // Returns 0, buffers 0xAA
        REQUIRE(read1.has_value());
        REQUIRE(read1.value() == 0x00);

        auto read2 = ppu.ReadPPUDATA();  // Returns 0xAA, buffers 0xAA (still at 0x100 due to /2)
        REQUIRE(read2.has_value());
        REQUIRE(read2.value() == 0xAA);

        auto read3 = ppu.ReadPPUDATA();  // Returns 0xAA, buffers 0xBB
        REQUIRE(read3.has_value());
        REQUIRE(read3.value() == 0xAA);
        
        auto read4 = ppu.ReadPPUDATA();  // Returns 0xBB, buffers 0xBB
        REQUIRE(read4.has_value());
        REQUIRE(read4.value() == 0xBB);
        
        auto read5 = ppu.ReadPPUDATA();  // Returns 0xBB, buffers 0xCC
        REQUIRE(read5.has_value());
        REQUIRE(read5.value() == 0xBB);
        
        auto read6 = ppu.ReadPPUDATA();  // Returns 0xCC
        REQUIRE(read6.has_value());
        REQUIRE(read6.value() == 0xCC);
      }
    }

    WHEN("reading from address 0x2000 (start of VRAM)") {
      ppu.WriteToVRAM(0, 0x55);

      ppu.WritePPUADDR(0x20);
      ppu.WritePPUADDR(0x00);

      THEN("the read should access VRAM correctly") {
        auto first_read = ppu.ReadPPUDATA();  // Returns 0, buffers 0x55
        REQUIRE(first_read.has_value());
        REQUIRE(first_read.value() == 0x00);
        
        auto second_read = ppu.ReadPPUDATA();  // Returns 0x55
        REQUIRE(second_read.has_value());
        REQUIRE(second_read.value() == 0x55);
      }
    }

    WHEN("reading from address 0x2FFF (end of VRAM)") {
      ppu.WriteToVRAM(0x7FF, 0x99);

      ppu.WritePPUADDR(0x2F);
      ppu.WritePPUADDR(0xFE);  // Use 0x2FFE instead of 0x2FFF to avoid overflow into palette region

      THEN("the read should access VRAM correctly") {
        auto first_read = ppu.ReadPPUDATA();  // Returns 0, buffers 0x99
        REQUIRE(first_read.has_value());
        REQUIRE(first_read.value() == 0x00);
        
        auto second_read = ppu.ReadPPUDATA();  // Returns 0x99
        REQUIRE(second_read.has_value());
        REQUIRE(second_read.value() == 0x99);
      }
    }

    WHEN("reading from palette table at 0x3F00") {
      ppu.WriteToPalette(0, 0x77);

      ppu.WritePPUADDR(0x3F);
      ppu.WritePPUADDR(0x00);

      THEN("the read should return buffered behavior") {
        auto first_read = ppu.ReadPPUDATA();  // Returns 0, buffers 0x77
        REQUIRE(first_read.has_value());
        REQUIRE(first_read.value() == 0x00);
        
        auto second_read = ppu.ReadPPUDATA();  // Returns 0x77
        REQUIRE(second_read.has_value());
        REQUIRE(second_read.value() == 0x77);
      }
    }

    WHEN("reading from palette table at 0x3F1F (within addressable palette)") {
      ppu.WriteToPalette(0x1F, 0x88);
      
      ppu.WritePPUADDR(0x3F);
      ppu.WritePPUADDR(0x1F);

      THEN("the read should succeed") {
        auto first_read = ppu.ReadPPUDATA();  // Returns 0, buffers 0x88
        REQUIRE(first_read.has_value());
        REQUIRE(first_read.value() == 0x00);
        
        auto second_read = ppu.ReadPPUDATA();  // Returns 0x88
        REQUIRE(second_read.has_value());
        REQUIRE(second_read.value() == 0x88);
      }
    }
  }
}
