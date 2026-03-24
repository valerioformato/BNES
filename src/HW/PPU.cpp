//
// Created by Valerio Formato on 23-Jan-26.
//

#include "HW/PPU.h"
#include "HW/Constants.h"
#include "common/ranges_compat.h"

#include <bit>
#include <bitset>
#include <cstring>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace BNES::HW {

std::shared_ptr<spdlog::logger> PPU::s_logger = []() {
  auto logger = spdlog::get("PPU");
  if (!logger) {
    try {
      logger = spdlog::stdout_color_st("PPU");
    } catch (const spdlog::spdlog_ex &) {
      // Logger already exists, retrieve it
      logger = spdlog::get("PPU");
    }
  }
  return logger;
}();

std::span<const uint8_t> PPU::ActiveNametable() const {
  uint8_t nametable_index = BaseNametableAddress();
  return Nametable(nametable_index);
}

std::span<const uint8_t> PPU::Nametable(uint8_t nametable_index) const {
  // Compute the VRAM index for the selected nametable and return a span into m_vram.
  Addr start_addr = VRAM_START_ADDRESS + static_cast<Addr>(nametable_index) * 0x400;
  Addr vram_index = MirrorVRAMAddress(start_addr);
  return std::span<const uint8_t>(m_vram).subspan(vram_index, 0x400);
}

std::span<const uint8_t, 4> PPU::BackgroundPalette(uint8_t index) const {
  return std::span<const uint8_t, 4>{std::next(m_palette_table.cbegin(), 4 * index), size_t{4}};
}

std::span<const uint8_t, 4> PPU::SpritePalette(uint8_t index) const {
  return std::span<const uint8_t, 4>{std::next(m_palette_table.cbegin(), 0x10 + 4 * index), size_t{4}};
}

std::array<PPU::SpriteData, 64> PPU::SpriteOAMData() const {
  std::array<SpriteData, 64> result;
  std::memcpy(result.data(), m_oam_data.data(), m_oam_data.size());
  return result;
}

PPU::TilePixelValues PPU::DecodeTile(std::span<const uint8_t> tile_chr_data) {
  TilePixelValues tile_pixels_v{0};

  for (const auto [index, byte] : rv::enumerate(tile_chr_data)) {
    auto row_index = index % TILE_WIDTH;
    auto bit_pos = index / TILE_WIDTH;

    for (size_t x = 0; x < TILE_WIDTH; ++x) {
      bool value = std::bitset<8>(byte)[TILE_WIDTH - x - 1];
      tile_pixels_v[row_index * TILE_WIDTH + x] |= (value << bit_pos);
    }
  }

  return tile_pixels_v;
}

void PPU::UpdateSprite0Hit(unsigned int cycles_to_advance) {
  // NOTE: from NESDev:
  //       While the PPU is drawing the picture, when an opaque pixel of sprite 0 overlaps an opaque pixel of the
  //       background, this is a sprite 0 hit. The PPU detects this condition and sets bit 6 of PPUSTATUS ($2002) to 1
  //       starting at this pixel, letting the CPU know how far along the PPU is in drawing the picture.

  // check if sprite0 hit flag is already set
  if (m_status_register & 0b01000000) {
    return;
  }

  // sprite 0 hit cannot occur if background or sprite rendering is disabled
  if (!RenderBackground() || !RenderSprites()) {
    return;
  }

  unsigned int current_x = m_cycles;
  unsigned int current_y = m_current_scanline;
  unsigned int target_x = m_cycles + cycles_to_advance;
  unsigned int target_y = m_current_scanline;
  if (target_x > 341) {
    target_x -= 341;
    target_y += 1;
  }

  // Get position for sprite 0
  SpriteData sprite0;
  memcpy(&sprite0, m_oam_data.data(), sizeof(sprite0));

  unsigned int sprite0_pos_x = sprite0.pos_x;
  unsigned int sprite0_pos_y = sprite0.pos_y;

  auto is_position_on_sprite = [](unsigned int x, unsigned int y, const SpriteData &sprite) {
    // pos_y in OAM is stored as actual_y - 1 (delayed by one scanline)
    unsigned int sprite_top = sprite.pos_y + 1;
    return x >= sprite.pos_x && x < sprite.pos_x + TILE_WIDTH && y >= sprite_top && y < sprite_top + TILE_HEIGHT;
  };

  // are we intersecting sprite0?
  if (is_position_on_sprite(current_x, current_y, sprite0) || is_position_on_sprite(target_x, target_y, sprite0)) {
    for (unsigned int i_pix = 0; i_pix < cycles_to_advance; ++i_pix) {
      auto x = current_x + i_pix;
      auto y = current_y;
      if (x > 341) {
        x -= 341;
        y += 1;
      }

      if (!is_position_on_sprite(x, y, sprite0)) {
        continue;
      }

      // sprite 0 hit never occurs at x=255, or at x=0..7 when left-side clipping is active
      if (x == 255 || (x <= 7 && (!ShowBackgroundLeftBorder() || !ShowSpritesLeftBorder()))) {
        continue;
      }

      auto chr_tiles = CharacterRom() | rv::chunk(TILE_MEMORY_SIZE);

      const auto nametable = ActiveNametable().subspan(0, 960);
      unsigned int bkg_tile_idx = (x / TILE_WIDTH) + (y / TILE_HEIGHT) * 32;
      const auto bkg_tile = chr_tiles[256 * BankIndex() + nametable[bkg_tile_idx]];
      TilePixelValues bkg_tile_data = DecodeTile(bkg_tile);

      const auto sprite_raw = chr_tiles[256 * SpritePatternTableAddress() + sprite0.tile_index];
      TilePixelValues sprite_data = DecodeTile(sprite_raw);

      const auto bkg_pos_x = (bkg_tile_idx * TILE_WIDTH) % NES_SCREEN_W;
      const auto bkg_pos_y = (bkg_tile_idx / (NES_SCREEN_W / TILE_WIDTH)) * TILE_HEIGHT;

      auto sprite_pix_idx = x - sprite0.pos_x + (y - (sprite0.pos_y + 1)) * TILE_WIDTH;
      auto bkg_tile_pix_idx = x - bkg_pos_x + (y - bkg_pos_y) * TILE_WIDTH;

      // finally, let's check if both pixels are opaque and set the sprite0 hit flag
      if (sprite_data[sprite_pix_idx] && bkg_tile_data[bkg_tile_pix_idx]) {
        m_status_register |= 0b01000000;
      }
    }
  }
}

void PPU::Tick(unsigned int cycles) {
  static std::chrono::time_point<std::chrono::steady_clock> last_time = std::chrono::steady_clock::now();

  UpdateSprite0Hit(cycles);

  m_cycles += cycles;

  // NOTE from https://www.nesdev.org/wiki/PPU_registers#OAMADDR
  // OAMADDR is set to 0 during each of ticks 257–320 (the sprite tile loading interval) of the pre-render and
  // visible scanlines. This also means that at the end of a normal complete rendered frame, OAMADDR will always have
  // returned to 0.

  if ((m_current_scanline == 261 || m_current_scanline < 241) && (m_cycles > 256 && m_cycles < 321)) {
    m_oam_address = 0;
  }

  while (m_cycles >= 341) {
    m_cycles -= 341;
    m_current_scanline += 1;

    if (m_current_scanline == 241) {
      m_status_register |= 0b10000000;
      if (VblankNMIEnabled()) {
        const auto now = std::chrono::steady_clock::now();
        m_last_frame_time = now - last_time;
        last_time = now;

        m_bus->PropagateNMI();
        m_cycles += 7 * 3; // NMI takes 2 CPU cycles to process
      }
    }

    if (m_current_scanline == 261) {
      // on the pre-render scanline we reset the sprite-0 hit flag
      m_status_register &= 0b10111111;
    }

    if (m_current_scanline >= 262) {
      m_current_scanline = 0;
      m_status_register &= ~0b10000000;
    }
  }
}

void PPU::WritePPUADDR(uint8_t value) {
  // W = 0 means first write, where high bits are written
  if (m_internal_registers[Register::W] == 0) {
    m_address_register = (value & 0x3F) << 8;
  } else {
    m_address_register |= value;
  }

  // In HW the value of PPUADDR is actually held in the t register, while outside of rendering.
  // For now we keep both.
  m_internal_registers[Register::T] = m_address_register;
  m_internal_registers[Register::W] = 1 - m_internal_registers[Register::W];
}

void PPU::WritePPUSCROLL(uint8_t value) {
  // W = 0 means first write, where high bits are written
  if (m_internal_registers[Register::W] == 0) {
    m_ppu_scroll_x = value | ((m_control_register & 0x1) << 8);
  } else {
    m_ppu_scroll_y = value | ((m_control_register & 0x2) << 8);
  }

  m_internal_registers[Register::W] = 1 - m_internal_registers[Register::W];
}

void PPU::WritePPUCTRL(uint8_t value) {
  // TODO: After power/reset, writes to this register are ignored until the first
  // pre-render scanline.

  bool last_vblank_nmi_enabled = VblankNMIEnabled();

  m_control_register = value;

  if (VRAMAddressIncrement()) {
    m_vram_address_increment = 32;
  } else {
    m_vram_address_increment = 1;
  }

  // In addition to scanline position, PPU would immediately trigger NMI if both of these conditions are met:
  //  - PPU is VBLANK state
  //  - "Generate NMI" bit in the control Register is updated from 0 to 1.
  if (!last_vblank_nmi_enabled && VblankNMIEnabled() && IsInVblank()) {
    m_bus->PropagateNMI();
  }
}

PPU::Addr PPU::MirrorVRAMAddress(Addr address) const {
  Addr mirrored_vram = address & 0b10111111111111;
  Addr vram_index = mirrored_vram - VRAM_START_ADDRESS;
  uint8_t name_table = vram_index / 0x400;

  if (m_mirroring == Rom::Mirroring::Vertical && (name_table == 2 || name_table == 3)) {
    return vram_index - 0x800;
  }

  if (m_mirroring == Rom::Mirroring::Horizontal && (name_table == 2 || name_table == 1)) {
    return vram_index - 0x400;
  }

  if (m_mirroring == Rom::Mirroring::Horizontal && name_table == 3) {
    return vram_index - 0x800;
  }

  return vram_index;
}

void PPU::WritePPUDATA(uint8_t value) {
  if (m_address_register < VRAM_START_ADDRESS) {
    throw std::runtime_error(
        fmt::format("Invalid write to PPUADDR value {} - Inside character ROM", m_address_register));
  }

  if (m_address_register >= VRAM_START_ADDRESS && m_address_register <= MAX_ADDRESSABLE_VRAM_ADDRESS) {
    m_vram[MirrorVRAMAddress(m_address_register)] = value;
  } else if (m_address_register >= PALETTE_TABLE_START_ADDRESS &&
             m_address_register <= MAX_ADDRESSABLE_PALETTE_TABLE_ADDRESS) {
    uint16_t palette_offset = (m_address_register - PALETTE_TABLE_START_ADDRESS) % 0x20;
    m_palette_table[palette_offset] = value;
  }

  m_address_register += m_vram_address_increment;
}

void PPU::WritePPUMASK(uint8_t value) {
  // TODO: After power/reset, writes to this register are ignored until the first pre-render scanline.
  m_mask_register = value;
}

void PPU::WriteOAMADDR(uint8_t value) { m_oam_address = value; }

void PPU::WriteOAMDATA(uint8_t value) {
  // https://www.nesdev.org/wiki/PPU_registers#OAMDATA
  // Writes will increment OAMADDR after the write.
  // ****BUT****
  // Writes to OAMDATA during rendering (on the pre-render line and the visible lines 0–239, provided either sprite or
  // background rendering is enabled) do not modify values in OAM, but do perform a glitchy increment of OAMADDR,
  // bumping only the high 6 bits (i.e., it bumps the [n] value in PPU sprite evaluation – it's plausible that it could
  // bump the low bits instead depending on the current status of sprite evaluation). This extends to DMA transfers via
  // OAMDMA, since that uses writes to $2004. For emulation purposes, it is probably best to completely ignore writes
  // during rendering.

  if (!RenderingInProgress()) {
    m_oam_data[m_oam_address] = value;
    m_oam_address = (m_oam_address + 1);
  } else {
    m_oam_address = (m_oam_address + 4);
  }
}

void PPU::OAMDMATransfer(std::span<const uint8_t, 256> oam_data) { rg::copy(oam_data, m_oam_data.begin()); }

uint8_t PPU::ReadPPUDATA() {
  uint8_t value_to_return{m_read_buffer};

  if (m_address_register <= MAX_ADDRESSABLE_CHR_ROM_ADDRESS) {
    m_read_buffer = m_character_rom[m_address_register];
  } else if (m_address_register >= VRAM_START_ADDRESS && m_address_register <= MAX_ADDRESSABLE_VRAM_ADDRESS) {
    m_read_buffer = m_vram[MirrorVRAMAddress(m_address_register)];
  } else if (m_address_register >= PALETTE_TABLE_START_ADDRESS &&
             m_address_register <= MAX_ADDRESSABLE_PALETTE_TABLE_ADDRESS) {
    uint16_t palette_offset = (m_address_register - PALETTE_TABLE_START_ADDRESS) % 0x20;
    value_to_return = m_palette_table[palette_offset];
    // Palette reads also update the buffer with the "underneath" nametable memory
    // $3Fxx maps to $2Fxx underneath (following $3xxx -> $2xxx mirror pattern)

    // TODO: check this if possible. Not sure how this interacts with different mirrorings
    Addr underlying_address = m_address_register - 0x1000;
    m_read_buffer = m_vram[MirrorVRAMAddress(underlying_address)];
  } else {
    throw std::runtime_error(fmt::format("Invalid read of PPUADDR value {}", m_address_register));
  }

  m_address_register += m_vram_address_increment;
  return value_to_return;
}

uint8_t PPU::ReadPPUSTATUS() {
  m_internal_registers[Register::W] = 0;

  auto value_to_return = m_status_register;
  m_status_register &= 0b01111111;

  return value_to_return;
}

uint8_t PPU::ReadOAMDATA() { return m_oam_data[m_oam_address]; }

} // namespace BNES::HW
