//
// Created by vformato on 12/23/25.
//

#include "HW/Bus.h"
#include "common/Utils.h"

#include <spdlog/spdlog.h>

namespace BNES::HW {
uint8_t Bus::Read(Addr address) {
  if (address <= MAX_ADDRESSABLE_RAM_ADDRESS) {
    // mask out bit 12 and 13 to simulate mirroring
    address &= 0b11111111111;
    return m_ram[address];
  } else if (address >= PPU_START_REGISTER && address <= MAX_ADDRESSABLE_PPU_ADDRESS) {
    TODO("PPU not implemented yet");
  } else if (address >= ROM_START_REGISTER && address <= MAX_ADDRESSABLE_ROM_ADDRESS) {
    Addr rom_address = address - ROM_START_REGISTER;
    // check if mirroring is needed
    if (m_rom.program_rom.size() == 0x4000 && address >= 0x4000) {
      rom_address %= 0x4000;
    }
    return m_rom.program_rom[rom_address];
  }

  spdlog::error("Bus read request for address {}: Address out of range", address);
  return 0;
}

void Bus::Write(Addr address, uint8_t data) {
  if (address <= MAX_ADDRESSABLE_RAM_ADDRESS) {
    // mask out bit 12 and 13 to simulate mirroring
    address &= 0b11111111111;
    m_ram[address] = data;

    return;
  } else if (address >= PPU_START_REGISTER && address <= MAX_ADDRESSABLE_PPU_ADDRESS) {
    TODO("PPU not implemented yet");
  } else if (address >= ROM_START_REGISTER && address <= MAX_ADDRESSABLE_ROM_ADDRESS) {
    spdlog::error("CANNOT WRITE TO ROM MEMORY!!!");
    throw std::runtime_error("Can't write to ROM memory");
  }

  spdlog::error("Bus write request for address {}: Address out of range", address);
}

} // namespace BNES::HW
