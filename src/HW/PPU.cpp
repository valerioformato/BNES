//
// Created by Valerio Formato on 23-Jan-26.
//

#include "PPU.h"

namespace BNES::HW {

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

void PPU::WritePPUCTRL(uint8_t value) {
  // TODO: After power/reset, writes to this register are ignored until the first
  // pre-render scanline.

  m_control_register = value;

  if (VRAMAddressIncrement()) {
    m_vram_address_increment = 32;
  } else {
    m_vram_address_increment = 1;
  }
}

ErrorOr<uint8_t> PPU::ReadPPUDATA() {
  uint8_t value_to_return{m_read_buffer};

  auto mirrored_address = [this](Addr address) -> Addr {
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

  if (m_address_register <= MAX_ADDRESSABLE_CHR_ROM_ADDRESS) {
    m_read_buffer = m_character_rom[m_address_register];
  } else if (m_address_register >= VRAM_START_ADDRESS && m_address_register <= MAX_ADDRESSABLE_VRAM_ADDRESS) {
    m_read_buffer = m_vram[mirrored_address(m_address_register)];
  } else if (m_address_register >= PALETTE_TABLE_START_ADDRESS &&
             m_address_register <= MAX_ADDRESSABLE_PALETTE_TABLE_ADDRESS) {
    uint16_t palette_offset = (m_address_register - PALETTE_TABLE_START_ADDRESS) % 0x20;
    value_to_return = m_palette_table[palette_offset];
    // Palette reads also update the buffer with the "underneath" nametable memory
    // $3Fxx maps to $2Fxx underneath (following $3xxx -> $2xxx mirror pattern)

    // TODO: check this if possible. Not sure how this interacts with different mirrorings
    Addr underlying_address = m_address_register - 0x1000;
    m_read_buffer = m_vram[mirrored_address(underlying_address)];
  } else {
    throw std::runtime_error(fmt::format("Invalid read of PPUADDR value {}", m_address_register));
  }

  m_address_register += m_vram_address_increment;
  return value_to_return;
}
} // namespace BNES::HW