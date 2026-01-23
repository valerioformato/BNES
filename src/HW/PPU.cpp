//
// Created by Valerio Formato on 23-Jan-26.
//

#include "PPU.h"

namespace BNES::HW {

void PPU::WritePPUADDR(uint8_t value) {
  // W = 0 means first write, where high bits are written
  if (m_internal_registers[Register::W] == 0) {
    m_mmio_registers[MMIORegister::Address] = (value & 0x3F) << 8;
  } else {
    m_mmio_registers[MMIORegister::Address] |= value;
  }

  // In HW the value of PPUADDR is actually held in the t register, while outside of rendering.
  // For now we keep both.
  m_internal_registers[Register::T] = m_mmio_registers[MMIORegister::Address];
  m_internal_registers[Register::W] = 1 - m_internal_registers[Register::W];
}

} // namespace BNES::HW