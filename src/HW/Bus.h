//
// Created by vformato on 12/23/25.
//

#ifndef BUS_H
#define BUS_H

#include <array>
#include <cstddef>
#include <cstdint>

namespace BNES::HW {
class Bus {
public:
  using Addr = std::uint16_t;

  static constexpr size_t RAM_MEM_SIZE = 0x800;
  static constexpr size_t MAX_ADDRESSABLE_RAM_ADDRESS = 0x1FFF;
  static constexpr size_t PPU_START_REGISTER = 0x2000;
  static constexpr size_t MAX_ADDRESSABLE_PPU_ADDRESS = 0x3FFF;

  [[nodiscard]] uint8_t Read(Addr address);
  void Write(Addr address, uint8_t data);

private:
  std::array<uint8_t, RAM_MEM_SIZE> m_ram{0};
};
} // namespace BNES::HW

#endif // BUS_H
