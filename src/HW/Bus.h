//
// Created by vformato on 12/23/25.
//

#ifndef BUS_H
#define BUS_H

#include "HW/Rom.h"
#include "common/Types/non_owning_ptr.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace BNES::HW {
class Bus {
  // For unit tests
  friend class CPUMock;

public:
  using Addr = std::uint16_t;

  static constexpr size_t RAM_MEM_SIZE = 0x800;
  static constexpr size_t MAX_ADDRESSABLE_RAM_ADDRESS = 0x1FFF;
  static constexpr size_t PPU_START_REGISTER = 0x2000;
  static constexpr size_t MAX_ADDRESSABLE_PPU_ADDRESS = 0x3FFF;
  static constexpr size_t ROM_START_REGISTER = 0x8000;
  static constexpr size_t MAX_ADDRESSABLE_ROM_ADDRESS = 0xFFFF;

  [[nodiscard]] ErrorOr<void> LoadRom(std::string_view rom_file) {
    m_rom = TRY(Rom::FromFile(rom_file));
    return {};
  }

  [[nodiscard]] uint8_t Read(Addr address);
  void Write(Addr address, uint8_t data);

  // Used mainly in unit tests...
  ErrorOr<void> LoadIntoProgramRom(std::span<const uint8_t> program);

  [[nodiscard]] const ::BNES::HW::Rom &Rom() const { return m_rom; };

private:
  std::array<uint8_t, RAM_MEM_SIZE> m_ram{0};
  ::BNES::HW::Rom m_rom;
};
} // namespace BNES::HW

#endif // BUS_H
