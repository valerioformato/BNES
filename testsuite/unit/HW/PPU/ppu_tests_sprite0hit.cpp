//
// Created by Valerio Formato on 12-Mar-26.
//

#include "HW/PPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

class PPUMock : public PPU {
public:
  using PPU::OAMDMATransfer;
  using PPU::PPU;
  using PPU::ReadPPUSTATUS;
  using PPU::Tick;
  using PPU::WritePPUCTRL;
  using PPU::WritePPUMASK;

  void WriteToVRAM(Addr addr, uint8_t value) { m_vram[addr] = value; }
};

static constexpr unsigned int CYCLES_PER_SCANLINE = 341;
static constexpr size_t CHR_ROM_SIZE = 256 * PPU::TILE_MEMORY_SIZE; // one bank, 4096 bytes

// Fill every pixel of a tile with a non-zero value (opaque)
static void SetOpaqueTile(std::vector<uint8_t> &chr_rom, int tile_idx) {
  auto offset = tile_idx * PPU::TILE_MEMORY_SIZE;
  std::fill(chr_rom.begin() + offset, chr_rom.begin() + offset + 8, 0xFF); // plane 0 all 1s
  // plane 1 (bytes 8-15) remains 0x00 → pixel value = 0b01 everywhere (opaque)
}

// Make a tile with exactly one opaque pixel at (pixel_x, pixel_y); all others transparent
static void SetSinglePixelOpaqueTile(std::vector<uint8_t> &chr_rom, int tile_idx, int pixel_x, int pixel_y) {
  // In NES CHR, pixel at column x in a row byte is bit (7 - x)
  auto offset = tile_idx * PPU::TILE_MEMORY_SIZE;
  chr_rom[offset + pixel_y] = static_cast<uint8_t>(0x80u >> pixel_x);
  // plane 1 remains 0x00 → pixel value = 0b01 at (pixel_x, pixel_y) only
}

static std::array<uint8_t, 256> MakeOAM(uint8_t screen_x, uint8_t screen_y, uint8_t tile_idx = 0) {
  std::array<uint8_t, 256> oam{};
  oam[0] = static_cast<uint8_t>(screen_y - 1); // OAM Y is stored as actual_y - 1
  oam[1] = tile_idx;
  oam[2] = 0x00; // attributes
  oam[3] = screen_x;
  return oam;
}

// ─── Basic hit detection ──────────────────────────────────────────────────────

SCENARIO("Sprite 0 hit flag detection", "[PPU][Sprite0]") {

  GIVEN("sprite 0 and the background tile both have opaque pixels at the same position") {
    Bus bus;
    std::vector<uint8_t> chr(CHR_ROM_SIZE, 0x00);
    SetOpaqueTile(chr, 0); // tile 0: fully opaque
    REQUIRE(bus.LoadIntoChrRom(chr).has_value());
    PPUMock ppu{bus};
    ppu.WritePPUMASK(0b00011000); // enable background + sprite rendering

    // Sprite 0 at screen position (40, 40): OAM stores y - 1 = 39
    auto oam = MakeOAM(40, 40, 0);
    ppu.OAMDMATransfer(std::span<const uint8_t, 256>{oam});

    // Background tile at (40/8=5, 40/8=5) → nametable index 165 → default value 0 (opaque tile 0)
    // m_vram is zero-initialised; no explicit write needed

    ppu.Tick(40 * CYCLES_PER_SCANLINE); // advance to scanline 40, cycle 0

    WHEN("the PPU advances through the overlapping pixel at x=40") {
      ppu.Tick(41); // processes cycles 0..40 at scanline 40; overlap occurs at x=40

      THEN("the sprite 0 hit flag should be set in PPUSTATUS") {
        REQUIRE((ppu.ReadPPUSTATUS() & 0b01000000) != 0);
      }
    }

    WHEN("background rendering is disabled in PPUMASK") {
      ppu.WritePPUMASK(0b00010000); // sprites only
      ppu.Tick(41);

      THEN("the sprite 0 hit flag should not be set") {
        REQUIRE((ppu.ReadPPUSTATUS() & 0b01000000) == 0);
      }
    }

    WHEN("sprite rendering is disabled in PPUMASK") {
      ppu.WritePPUMASK(0b00001000); // background only
      ppu.Tick(41);

      THEN("the sprite 0 hit flag should not be set") {
        REQUIRE((ppu.ReadPPUSTATUS() & 0b01000000) == 0);
      }
    }
  }

  // ─── Transparent pixels ──────────────────────────────────────────────────────

  GIVEN("sprite 0 uses a fully transparent tile at the overlap position") {
    Bus bus;
    std::vector<uint8_t> chr(CHR_ROM_SIZE, 0x00);
    SetOpaqueTile(chr, 0); // tile 0: opaque  (used for background)
    // tile 1 stays all zeros → transparent (used for sprite)
    REQUIRE(bus.LoadIntoChrRom(chr).has_value());
    PPUMock ppu{bus};
    ppu.WritePPUMASK(0b00011000);

    auto oam = MakeOAM(40, 40, 1); // sprite uses tile 1 (transparent)
    ppu.OAMDMATransfer(std::span<const uint8_t, 256>{oam});
    // nametable[165] = 0 → background uses tile 0 (opaque)

    ppu.Tick(40 * CYCLES_PER_SCANLINE);

    WHEN("the PPU advances through the overlap position") {
      ppu.Tick(41);

      THEN("the sprite 0 hit flag should not be set") {
        REQUIRE((ppu.ReadPPUSTATUS() & 0b01000000) == 0);
      }
    }
  }

  GIVEN("the background tile at the overlap position is fully transparent") {
    Bus bus;
    std::vector<uint8_t> chr(CHR_ROM_SIZE, 0x00);
    SetOpaqueTile(chr, 0); // tile 0: opaque  (used for sprite)
    // tile 1 stays all zeros → transparent (used for background)
    REQUIRE(bus.LoadIntoChrRom(chr).has_value());
    PPUMock ppu{bus};
    ppu.WritePPUMASK(0b00011000);

    auto oam = MakeOAM(40, 40, 0); // sprite uses tile 0 (opaque)
    ppu.OAMDMATransfer(std::span<const uint8_t, 256>{oam});
    ppu.WriteToVRAM(165, 1); // nametable[165] = tile 1 (transparent)

    ppu.Tick(40 * CYCLES_PER_SCANLINE);

    WHEN("the PPU advances through the overlap position") {
      ppu.Tick(41);

      THEN("the sprite 0 hit flag should not be set") {
        REQUIRE((ppu.ReadPPUSTATUS() & 0b01000000) == 0);
      }
    }
  }

  // ─── x=255 exclusion ─────────────────────────────────────────────────────────

  GIVEN("sprite 0 has an opaque pixel only at x=255 (x_offset=7 of a sprite starting at x=248)") {
    Bus bus;
    std::vector<uint8_t> chr(CHR_ROM_SIZE, 0x00);
    // Tile 0: opaque only at x_offset=7, row 0 → screen x=255 for sprite at x=248
    SetSinglePixelOpaqueTile(chr, 0, 7, 0);
    REQUIRE(bus.LoadIntoChrRom(chr).has_value());
    PPUMock ppu{bus};
    ppu.WritePPUMASK(0b00011000);

    // Sprite at x=248 → pixel 7 maps to screen x=255
    auto oam = MakeOAM(248, 40, 0);
    ppu.OAMDMATransfer(std::span<const uint8_t, 256>{oam});
    // Background tile at (248/8=31, 40/8=5) → nametable index 191 → default 0 (same tile)

    // Advance to scanline 40, cycle 248 in a single large tick to avoid the outer
    // boundary check triggering early in the loop
    ppu.Tick(40 * CYCLES_PER_SCANLINE + 248);

    WHEN("the PPU advances through x=248..255") {
      ppu.Tick(8); // current_x=248; processes x=248..255 at scanline 40

      THEN("the sprite 0 hit flag should not be set because x=255 is excluded") {
        REQUIRE((ppu.ReadPPUSTATUS() & 0b01000000) == 0);
      }
    }
  }

  // ─── Left-side clipping ───────────────────────────────────────────────────────

  GIVEN("sprite 0 is positioned entirely within x=0..7") {
    Bus bus;
    std::vector<uint8_t> chr(CHR_ROM_SIZE, 0x00);
    SetOpaqueTile(chr, 0);
    REQUIRE(bus.LoadIntoChrRom(chr).has_value());

    // Background tile at (0/8=0, 40/8=5) → nametable index 160 → default 0 (opaque tile 0)
    auto oam = MakeOAM(0, 40, 0); // sprite at x=0, spans x=0..7

    WHEN("left-side clipping is active (ShowBackgroundLeftBorder and ShowSpritesLeftBorder are both 0)") {
      // PPUMASK 0b00011000: bits 1 (ShowBackgroundLeftBorder) and 2 (ShowSpritesLeftBorder) = 0
      PPUMock ppu{bus};
      ppu.WritePPUMASK(0b00011000);
      ppu.OAMDMATransfer(std::span<const uint8_t, 256>{oam});
      ppu.Tick(40 * CYCLES_PER_SCANLINE);
      ppu.Tick(1); // processes x=0 at scanline 40

      THEN("the sprite 0 hit flag should not be set") {
        REQUIRE((ppu.ReadPPUSTATUS() & 0b01000000) == 0);
      }
    }

    WHEN("left-side clipping is inactive (ShowBackgroundLeftBorder and ShowSpritesLeftBorder are both 1)") {
      // PPUMASK 0b00011110: bits 1 and 2 set → no clipping on left border
      PPUMock ppu{bus};
      ppu.WritePPUMASK(0b00011110);
      ppu.OAMDMATransfer(std::span<const uint8_t, 256>{oam});
      ppu.Tick(40 * CYCLES_PER_SCANLINE);
      ppu.Tick(1); // processes x=0 at scanline 40

      THEN("the sprite 0 hit flag should be set") {
        REQUIRE((ppu.ReadPPUSTATUS() & 0b01000000) != 0);
      }
    }
  }

  // ─── Flag reset on pre-render scanline ───────────────────────────────────────

  GIVEN("sprite 0 hit was detected during a frame") {
    Bus bus;
    std::vector<uint8_t> chr(CHR_ROM_SIZE, 0x00);
    SetOpaqueTile(chr, 0);
    REQUIRE(bus.LoadIntoChrRom(chr).has_value());
    PPUMock ppu{bus};
    ppu.WritePPUMASK(0b00011000);

    auto oam = MakeOAM(40, 40, 0);
    ppu.OAMDMATransfer(std::span<const uint8_t, 256>{oam});

    ppu.Tick(40 * CYCLES_PER_SCANLINE);
    ppu.Tick(41); // trigger the hit at x=40, scanline 40

    REQUIRE((ppu.ReadPPUSTATUS() & 0b01000000) != 0); // precondition: flag is set

    WHEN("the pre-render scanline (261) is reached") {
      // Advance from scanline 40, cycle 41 to scanline 261
      ppu.Tick((261 - 40) * CYCLES_PER_SCANLINE);

      THEN("the sprite 0 hit flag should be cleared") {
        REQUIRE((ppu.ReadPPUSTATUS() & 0b01000000) == 0);
      }
    }
  }
}
