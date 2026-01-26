//
// Created by Valerio Formato on 23-Jan-26.
//

#ifndef BNES_PPU_H
#define BNES_PPU_H

#include "HW/Bus.h"
#include "common/Types/EnumArray.h"

#include <array>
#include <bitset>
#include <cstdint>
#include <span>

namespace BNES::HW {

class PPU {
public:
  friend class Bus;

  static constexpr uint16_t MAX_ADDRESSABLE_CHR_ROM_ADDRESS = 0x1FFF;
  static constexpr uint16_t VRAM_START_ADDRESS = 0x2000;
  static constexpr uint16_t MAX_ADDRESSABLE_VRAM_ADDRESS = 0x2FFF;
  static constexpr uint16_t PALETTE_TABLE_START_ADDRESS = 0x3F00;
  static constexpr uint16_t MAX_ADDRESSABLE_PALETTE_TABLE_ADDRESS = 0x3FFF;

  using Addr = Bus::Addr;
  enum class Register { V = 0, T, X, W };

  PPU() = delete;
  explicit PPU(Bus &bus)
      : m_bus{&bus}, m_mirroring{m_bus->Rom().mirroring}, m_character_rom{m_bus->Rom().character_rom} {
    m_bus->AttachPPU(this);
  };

  [[nodiscard]] EnumArray<uint16_t, Register> InternalRegisters() const { return m_internal_registers; };

protected:
  void WritePPUADDR(uint8_t value);
  void WritePPUCTRL(uint8_t value);
  void WritePPUDATA(uint8_t value);
  void WritePPUMASK(uint8_t value);
  void WritePPUSCROLL(uint8_t value);
  void WriteOAMADDR(uint8_t value);
  void WriteOAMDATA(uint8_t value);

  [[nodiscard]] uint8_t ReadPPUDATA();
  [[nodiscard]] uint8_t ReadPPUSTATUS();
  [[nodiscard]] uint8_t ReadOAMDATA();

  // Getter for the PPUADDR register
  [[nodiscard]] uint16_t AddressRegister() const { return m_address_register; };

  // Helper getters for PPUCTRL settings
  [[nodiscard]] uint8_t BaseNametableAddress() const { return m_control_register & 0b00000011; };
  [[nodiscard]] bool VRAMAddressIncrement() const { return m_control_register & 0b00000100; };
  [[nodiscard]] bool SpritePatternTableAddress() const { return m_control_register & 0b00001000; };
  [[nodiscard]] bool BackgroundPatternTableAddress() const { return m_control_register & 0b00010000; };
  [[nodiscard]] bool SpriteSize() const { return m_control_register & 0b00100000; };
  [[nodiscard]] bool PPUMasterSlaveSelect() const { return m_control_register & 0b01000000; };
  [[nodiscard]] bool VblankNMIEnable() const { return m_control_register & 0b10000000; };

  // Protected members for testing
  std::array<uint8_t, 32> m_palette_table{0};
  std::array<uint8_t, 0x800> m_vram{0};

private:
  non_owning_ptr<Bus *> m_bus;

  Rom::Mirroring m_mirroring{Rom::Mirroring::Vertical};

  std::span<const uint8_t> m_character_rom;
  std::array<uint8_t, 256> m_oam_data{0};

  uint8_t m_control_register{0};
  Addr m_address_register{0};
  uint8_t m_status_register{0};
  uint8_t m_mask_register{0};
  uint8_t m_oam_address{0};
  uint16_t m_ppu_scroll_x{0};
  uint16_t m_ppu_scroll_y{0};

  EnumArray<uint16_t, Register> m_internal_registers{};

  Addr m_vram_address_increment{1};
  uint8_t m_read_buffer{0};

  Addr MirrorVRAMAddress(Addr address);
};

} // namespace BNES::HW

#endif // BNES_PPU_H
