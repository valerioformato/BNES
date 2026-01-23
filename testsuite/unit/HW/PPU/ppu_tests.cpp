//
// Created by Valerio Formato on 23-Jan-26.
//
#include "HW/PPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

class PPUMock : public PPU {
public:
  using PPU::PPU;
  using PPU::WritePPUADDR;
};

SCENARIO("NES PPU initialization", "[PPU]") {
  GIVEN("a new PPU instance") {

    Bus bus;
    PPU ppu{bus};

    WHEN("the PPU is initialized") {
      THEN("the PPU registers should be in their default state") {
        auto registers = ppu.InternalRegisters();
        REQUIRE(registers[PPU::Register::V] == 0x0000);
        REQUIRE(registers[PPU::Register::T] == 0x0000);
        REQUIRE(registers[PPU::Register::X] == 0x0000);
        REQUIRE(registers[PPU::Register::W] == 0x0000);

        auto mmio_registers = ppu.MMIORegisters();
        REQUIRE(mmio_registers[PPU::MMIORegister::Address] == 0x0000);
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
        auto mmio_registers = ppu.MMIORegisters();
        REQUIRE(mmio_registers[PPU::MMIORegister::Address] == 0x1200);

        auto internal_registers = ppu.InternalRegisters();
        REQUIRE(internal_registers[PPU::Register::T] == 0x1200);
        REQUIRE(internal_registers[PPU::Register::W] == 1);
      }
    }

    WHEN("writing to PPUADDR twice") {
      ppu.WritePPUADDR(0x3F);
      ppu.WritePPUADDR(0x20);

      THEN("the full 14-bit address should be set") {
        auto mmio_registers = ppu.MMIORegisters();
        REQUIRE(mmio_registers[PPU::MMIORegister::Address] == 0x3F20);

        auto internal_registers = ppu.InternalRegisters();
        REQUIRE(internal_registers[PPU::Register::T] == 0x3F20);
        REQUIRE(internal_registers[PPU::Register::W] == 0);
      }
    }

    WHEN("writing to PPUADDR with high bits set in first write") {
      ppu.WritePPUADDR(0xFF);

      THEN("the high bits should be masked to 6 bits") {
        auto mmio_registers = ppu.MMIORegisters();
        REQUIRE(mmio_registers[PPU::MMIORegister::Address] == 0x3F00);

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
        auto mmio_registers = ppu.MMIORegisters();
        REQUIRE(mmio_registers[PPU::MMIORegister::Address] == 0x2134);

        auto internal_registers = ppu.InternalRegisters();
        REQUIRE(internal_registers[PPU::Register::T] == 0x2134);
        REQUIRE(internal_registers[PPU::Register::W] == 0);
      }
    }
  }
}
