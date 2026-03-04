//
// Created by vformato on 12/23/25.
//

#ifndef ROM_H
#define ROM_H

#include "common/Utils.h"

#include <cstdint>
#include <string_view>
#include <vector>

namespace BNES::HW {

class Rom {
public:
  enum class Mirroring {
    Vertical,
    Horizontal,
    FourScreen,
  };

  enum class TimingMode {
    NTSC,
    PAL,
    MultiRegion,
    Dendy,
  };

  enum class ConsoleType {
    NES,
    VsSystem,
    Playchoice10,
    Extended,
  };

  std::vector<uint8_t> program_rom;
  std::vector<uint8_t> character_rom;
  uint16_t mapper{0};
  uint8_t submapper{0};
  Mirroring mirroring{Mirroring::Vertical};
  TimingMode timing{TimingMode::NTSC};
  ConsoleType console_type{ConsoleType::NES};
  size_t prg_ram_size{0};
  size_t prg_nvram_size{0};
  size_t chr_ram_size{0};
  size_t chr_nvram_size{0};
  uint8_t vs_system_type{0};
  uint8_t extended_console_type{0};
  uint8_t misc_rom_count{0};
  uint8_t expansion_device{0};
  bool is_nes_v2{false};

  static ErrorOr<Rom> FromFile(std::string_view path);
};
} // namespace BNES::HW

#endif // ROM_H
