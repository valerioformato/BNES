//
// Created by Valerio Formato on 23-Jan-26.
//

#ifndef BNES_PPU_H
#define BNES_PPU_H

#include "HW/Bus.h"
#include "common/Types/EnumArray.h"

#include <array>
#include <cstdint>
#include <span>

namespace BNES::HW {

class PPU {
public:
  friend class Bus;

  using Addr = Bus::Addr;
  enum class MMIORegister {
    Address,
  };
  enum class Register { V, T, X, W };

  PPU() = delete;
  explicit PPU(Bus &bus)
      : m_bus{&bus}, m_mirroring{m_bus->Rom().mirroring}, m_character_rom{m_bus->Rom().character_rom} {};

  [[nodiscard]] EnumArray<uint16_t, Register> InternalRegisters() const { return m_internal_registers; };
  [[nodiscard]] EnumArray<uint16_t, MMIORegister> MMIORegisters() const { return m_mmio_registers; };

private:
  non_owning_ptr<Bus *> m_bus;

  Rom::Mirroring m_mirroring{Rom::Mirroring::Vertical};

  std::span<const uint8_t> m_character_rom;
  std::array<uint8_t, 32> m_palette_table{0};
  std::array<uint8_t, 256> m_oam_data{0};

  EnumArray<uint16_t, Register> m_internal_registers{};
  EnumArray<uint16_t, MMIORegister> m_mmio_registers{};

protected:
  void WritePPUADDR(uint8_t value);
};

} // namespace BNES::HW

#endif // BNES_PPU_H
