//
// Created by Valerio Formato on 23-Jan-26.
//

#ifndef BNES_PPU_H
#define BNES_PPU_H

#include "HW/Bus.h"
#include "common/Types/EnumArray.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <array>
#include <cstdint>
#include <span>

namespace BNES::Tools {
class PPUDebugger;
}

namespace BNES::HW {
class PPU {
public:
  friend class ::BNES::Tools::PPUDebugger;
  friend class Bus;

  static constexpr unsigned int TILE_MEMORY_SIZE = 16;
  static constexpr unsigned int TILE_WIDTH = 8;
  static constexpr unsigned int TILE_HEIGHT = 8;

  static constexpr uint16_t MAX_ADDRESSABLE_CHR_ROM_ADDRESS = 0x1FFF;
  static constexpr uint16_t VRAM_START_ADDRESS = 0x2000;
  static constexpr uint16_t MAX_ADDRESSABLE_VRAM_ADDRESS = 0x2FFF;
  static constexpr uint16_t PALETTE_TABLE_START_ADDRESS = 0x3F00;
  static constexpr uint16_t MAX_ADDRESSABLE_PALETTE_TABLE_ADDRESS = 0x3FFF;

  using Addr = Bus::Addr;
  enum class Register { V = 0, T, X, W };

  struct SpriteData {
    uint8_t pos_y;
    uint8_t tile_index;
    uint8_t palette_idx : 2;
    uint8_t : 3; // unused
    uint8_t priority : 1;
    uint8_t flip_horizontal : 1;
    uint8_t flip_vertical : 1;
    uint8_t pos_x;
  };

  PPU() = delete;
  explicit PPU(Bus &bus) : m_bus{&bus} { m_bus->AttachPPU(this); };

  void Init() {
    m_mirroring = m_bus->Rom().mirroring;
    m_character_rom = m_bus->Rom().character_rom;
  }

  [[nodiscard]] EnumArray<uint16_t, Register> InternalRegisters() const { return m_internal_registers; };

  [[nodiscard]] bool IsGrayScale() const { return m_mask_register & 0b00000001; }
  [[nodiscard]] bool ShowBackgroundLeftBorder() const { return m_mask_register & 0b00000010; }
  [[nodiscard]] bool ShowSpritesLeftBorder() const { return m_mask_register & 0b00000100; }
  [[nodiscard]] bool RenderBackground() const { return m_mask_register & 0b00001000; }
  [[nodiscard]] bool RenderSprites() const { return m_mask_register & 0b00010000; }
  [[nodiscard]] bool SpritePatternTableAddress() const { return m_control_register & 0b00001000; };
  [[nodiscard]] uint8_t BaseNametableAddress() const { return m_control_register & 0b00000011; };

  [[nodiscard]] uint16_t CurrentScanline() const { return m_current_scanline; }
  [[nodiscard]] size_t Cycles() const { return m_cycles; }
  [[nodiscard]] uint8_t BankIndex() const { return (m_control_register & 0b00010000) != 0; }

  [[nodiscard]] std::span<const uint8_t> CharacterRom() const { return m_character_rom; }
  [[nodiscard]] std::span<const uint8_t> ActiveNametable() const;
  [[nodiscard]] std::span<const uint8_t> Nametable(uint8_t index) const;
  [[nodiscard]] uint8_t BackgroundColor() const { return m_palette_table[0]; };
  [[nodiscard]] std::array<SpriteData, 64> SpriteOAMData() const;
  [[nodiscard]] std::span<const uint8_t, 4> BackgroundPalette(uint8_t index) const;
  [[nodiscard]] std::span<const uint8_t, 4> SpritePalette(uint8_t index) const;

  using TilePixelValues = std::array<uint8_t, TILE_WIDTH * TILE_HEIGHT>;
  static TilePixelValues DecodeTile(std::span<const uint8_t> tile_chr_data);

protected:
  void Tick(unsigned int cycles);

  void WritePPUADDR(uint8_t value);
  void WritePPUCTRL(uint8_t value);
  void WritePPUDATA(uint8_t value);
  void WritePPUMASK(uint8_t value);
  void WritePPUSCROLL(uint8_t value);
  void WriteOAMADDR(uint8_t value);
  void WriteOAMDATA(uint8_t value);
  void OAMDMATransfer(std::span<const uint8_t, 256> oam_data);

  [[nodiscard]] uint8_t ReadPPUDATA();
  [[nodiscard]] uint8_t ReadPPUSTATUS();
  [[nodiscard]] uint8_t ReadOAMDATA();

  // Getter for the PPUADDR register
  [[nodiscard]] uint16_t AddressRegister() const { return m_address_register; };

  // Helper getters for PPUCTRL settings
  [[nodiscard]] bool VRAMAddressIncrement() const { return m_control_register & 0b00000100; };
  [[nodiscard]] bool BackgroundPatternTableAddress() const { return m_control_register & 0b00010000; };
  [[nodiscard]] bool SpriteSize() const { return m_control_register & 0b00100000; };
  [[nodiscard]] bool PPUMasterSlaveSelect() const { return m_control_register & 0b01000000; };
  [[nodiscard]] bool VblankNMIEnabled() const { return m_control_register & 0b10000000; };
  [[nodiscard]] bool RenderingInProgress() const { return m_current_scanline < 240 || m_current_scanline == 261; }

  // Helper getters for PPUSTATUS register
  [[nodiscard]] bool IsInVblank() const { return m_status_register & 0b10000000; };

  void UpdateSprite0Hit(unsigned int cycles_to_advance);

  // Protected members for testing
  std::array<uint8_t, 32> m_palette_table{0};
  std::array<uint8_t, 0x800> m_vram{0};
  std::array<uint8_t, 256 * 240> m_screen_data{0};

private:
  size_t m_cycles{0};
  uint16_t m_current_scanline{0};

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

  std::chrono::duration<double> m_last_frame_time{0.0};

  static std::shared_ptr<spdlog::logger> s_logger;

  Addr MirrorVRAMAddress(Addr address) const;
};

} // namespace BNES::HW

#endif // BNES_PPU_H
