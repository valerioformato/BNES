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
  static uint8_t buffered_value{0};

  uint8_t value_to_return{buffered_value};

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
    buffered_value = m_character_rom[m_address_register];
  } else if (m_address_register >= VRAM_START_ADDRESS && m_address_register <= MAX_ADDRESSABLE_VRAM_ADDRESS) {
    // TODO: Check if mirroring correct
    buffered_value = m_vram[mirrored_address(m_address_register)];
  } else if (m_address_register > PALETTE_TABLE_START_ADDRESS &&
             m_address_register <= MAX_ADDRESSABLE_PALETTE_TABLE_ADDRESS) {
    buffered_value = m_palette_table[m_address_register - PALETTE_TABLE_START_ADDRESS];
  } else {
    throw std::runtime_error(fmt::format("Invalid read of PPUADDR value {}", m_address_register));
  }

  m_address_register += m_vram_address_increment;
   return value_to_return;
}
} // namespace BNES::HW