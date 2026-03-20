//
// Created by vformato on 12/23/25.
//

#include "HW/Bus.h"

#include "HW/CPU.h"
#include "HW/PPU.h"
#include "HW/Screen.h"
#include "Joypad.h"
#include "common/Utils.h"

#include <spdlog/spdlog.h>

namespace BNES::HW {

void Bus::Attach(CPU *cpu) { m_cpu = cpu; }
void Bus::Attach(PPU *ppu) { m_ppu = ppu; }
void Bus::Attach(Joypad *joypad, unsigned int joy_no) {
  switch (joy_no) {
  case 1:
    m_joypad1 = joypad;
    break;
  case 2:
    m_joypad2 = joypad;
    break;
  }
}
void Bus::Attach(Screen *screen) { m_screen = screen; }

void Bus::PropagateNMI() {
  m_cpu->ProcessNMI();

  // Only fill screen if a screen is attached.
  if (m_screen) {
    m_screen->FillFromPPU(*m_ppu);
  }
}

uint8_t Bus::Read(Addr address) {
  if (address <= MAX_ADDRESSABLE_RAM_ADDRESS) {
    // mask out bit 12 and 13 to simulate mirroring
    address &= 0b11111111111;
    return m_ram[address];
  }

  if (address == 0x2000 || address == 0x2001 || address == 0x2003 || address == 0x2005 || address == 0x2006 ||
      address == 0x4014) {
    spdlog::error("Bus read request for address 0x{:04X}: Address is write-only", address);
    throw std::runtime_error("Bus read request for write-only address");
  }
  if (address == 0x2002) {
    return m_ppu->ReadPPUSTATUS();
  }
  if (address == 0x2004) {
    return m_ppu->ReadOAMDATA();
  }
  if (address == 0x2007) {
    return m_ppu->ReadPPUDATA();
  }
  if (address >= 0x2008 && address <= MAX_ADDRESSABLE_PPU_ADDRESS) {
    // These are all mirrors of the PPU memory-mapped registers
    Addr mirror_down_addr = address & 0b0010000000000111;
    return Read(mirror_down_addr);
  }

  if (address >= ROM_START_REGISTER && address <= MAX_ADDRESSABLE_ROM_ADDRESS) {
    Addr rom_address = address - ROM_START_REGISTER;
    // check if mirroring is needed
    if (m_rom.program_rom.size() == 0x4000 && address >= 0x4000) {
      rom_address %= 0x4000;
    }
    return m_rom.program_rom[rom_address];
  }

  if (address == JOYPAD1_ADDRESS) {
    return m_joypad1->Read();
  }

  if (address == JOYPAD1_ADDRESS) {
    return m_joypad2->Read();
  }

  if (address >= PPU_START_REGISTER && address <= MAX_ADDRESSABLE_PPU_ADDRESS) {
    TODO(fmt::format("PPU read for address 0x{:04X} not implemented yet", address));
  }

  spdlog::error("Bus read request for address 0x{:04X}: Address out of range", address);
  return 0;
}

void Bus::Write(Addr address, uint8_t data) {
  if (address <= MAX_ADDRESSABLE_RAM_ADDRESS) {
    // mask out bit 12 and 13 to simulate mirroring
    address &= 0b11111111111;
    m_ram[address] = data;

    return;
  }

  if (address == 0x2000) {
    m_ppu->WritePPUCTRL(data);
  } else if (address == 0x2001) {
    m_ppu->WritePPUMASK(data);
  } else if (address == 0x2003) {
    m_ppu->WriteOAMADDR(data);
  } else if (address == 0x2004) {
    m_ppu->WriteOAMDATA(data);
  } else if (address == 0x2005) {
    m_ppu->WritePPUSCROLL(data);
  } else if (address == 0x2006) {
    m_ppu->WritePPUADDR(data);
  } else if (address == 0x2007) {
    m_ppu->WritePPUDATA(data);
  } else if (address >= 0x2008 && address <= MAX_ADDRESSABLE_PPU_ADDRESS) {
    Addr mirror_down_addr = address & 0b0010000000000111;
    Write(mirror_down_addr, data);
  } else if (address == 0x4014) { // OAMDMM transfer
    Addr starting_addr = 0x100 * data;
    auto oam_data =
        std::span<uint8_t, 256>{std::next(m_ram.begin(), starting_addr), std::next(m_ram.begin(), starting_addr + 256)};
    m_ppu->OAMDMATransfer(oam_data);

    // NOTE: from NESDEV
    //       Not counting the OAMDMA write tick, the above procedure takes 513 CPU cycles (+1 on odd CPU cycles): first
    //       one (or two) idle cycles, and then 256 pairs of alternating read/write cycles. (For comparison, an unrolled
    //       LDA/STA loop would usually take four times as long.)
    Tick((m_cpu->Cycles() % 2) ? 514 : 513);
  } else if (address >= PPU_START_REGISTER && address <= MAX_ADDRESSABLE_PPU_ADDRESS) {
    TODO(fmt::format("PPU write for address 0x{:04X} not implemented yet", address));
  } else if (address >= ROM_START_REGISTER && address <= MAX_ADDRESSABLE_ROM_ADDRESS) {
    spdlog::error("CANNOT WRITE TO ROM MEMORY!!!");
    throw std::runtime_error("Can't write to ROM memory");
  } else if (address == JOYPAD1_ADDRESS) {
    m_joypad1->Write(data);
  } else if (address == JOYPAD1_ADDRESS) {
    m_joypad2->Write(data);
  }

  else {
    spdlog::error("Bus write request for address 0x{:04X}: Address out of range", address);
  }
}

void Bus::Tick(unsigned int cycles) { m_ppu->Tick(cycles * PPU_FREQ_RATIO); }

ErrorOr<void> Bus::LoadIntoProgramRom(std::span<const uint8_t> program) {
  if (program.size() > (MAX_ADDRESSABLE_ROM_ADDRESS - ROM_START_REGISTER + 1)) {
    return make_error(std::make_error_code(std::errc::not_enough_memory), "Program too large to fit in memory");
  }

  if (program.size() > m_rom.program_rom.size()) {
    m_rom.program_rom.resize(program.size());
  }
  std::ranges::copy(program, m_rom.program_rom.begin());

  return {};
}

ErrorOr<void> Bus::LoadIntoChrRom(std::span<const uint8_t> chr_data) {
  if (chr_data.size() > (PPU_START_REGISTER - 1 + 1)) {
    return make_error(std::make_error_code(std::errc::not_enough_memory), "CHR data too large to fit in memory");
  }

  m_rom.character_rom.resize(chr_data.size());
  std::ranges::copy(chr_data, m_rom.character_rom.begin());

  return {};
}

} // namespace BNES::HW
