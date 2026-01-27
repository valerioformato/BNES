//
// Created by Valerio Formato on 23-Jan-26.
//

#include "PPU.h"

namespace BNES::HW {

void PPU::Tick(unsigned int cycles) {
  m_cycles += cycles;

  if (m_cycles >= 341) {
    m_cycles -= 341;
    m_current_scanline += 1;

    if (m_current_scanline == 241 && VblankNMIEnabled()) {
      m_status_register |= 0b1000000;
      m_bus->PropagateNMI();
    }

    if (m_current_scanline >= 262) {
      m_current_scanline = 0;
      m_status_register &= ~0b1000000;
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

PPU::Addr PPU::MirrorVRAMAddress(Addr address) {
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
};

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

void PPU::WriteOAMADDR(uint8_t value) {
  // NOTE from https://www.nesdev.org/wiki/PPU_registers#OAMADDR
  // OAMADDR is set to 0 during each of ticks 257–320 (the sprite tile loading interval) of the pre-render and
  // visible scanlines. This also means that at the end of a normal complete rendered frame, OAMADDR will always have
  // returned to 0.

  // if (... ) {
  //   m_oam_address = 0;
  // }

  m_oam_address = value;
}

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

  if (true /* !RenderingInProgress() */) {
    m_oam_data[m_oam_address] = value;
    m_oam_address = (m_oam_address + 1);
  } else {
    m_oam_address = (m_oam_address + 4);
  }
}

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
  return m_status_register;
}

uint8_t PPU::ReadOAMDATA() { return m_oam_data[m_oam_address]; }

} // namespace BNES::HW
