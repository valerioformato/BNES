//
// Created by vformato on 12/23/25.
//

#include "Rom.h"

#include <algorithm>
#include <filesystem>
#include <fstream>

namespace BNES::HW {
ErrorOr<Rom> Rom::FromFile(std::string_view path) {
  std::filesystem::path p(path);
  if (!std::filesystem::exists(p)) {
    return make_error(std::errc::no_such_file_or_directory, fmt::format("File {} not found", path));
  }

  std::ifstream input_file{path.data(), std::ios::binary};
  if (!input_file.is_open()) {
    return make_error(std::errc::io_error, fmt::format("Failed to open file {}", path));
  }

  // read first 4 bytes and check against the standard NES ROM header
  static constexpr std::array<uint8_t, 4> NES_TAG{0x4E, 0x45, 0x53, 0x1A};
  std::vector<uint8_t> header(NES_TAG.size());
  Utils::Raw::ReadNFromBinary(header, NES_TAG.size(), input_file);

  if (auto [r1, r2] = std::ranges::mismatch(NES_TAG, header); r1 != end(NES_TAG) && r2 != end(header)) {
    return make_error(std::errc::not_supported, fmt::format("Could not find NES header in file {}", path));
  }

  uint8_t n_prg_rom_banks = 0;
  Utils::Raw::ReadFromBinary(n_prg_rom_banks, input_file);

  uint8_t n_chr_rom_banks = 0;
  Utils::Raw::ReadFromBinary(n_chr_rom_banks, input_file);

  spdlog::debug("NES ROM banks: {}", n_prg_rom_banks);
  spdlog::debug("NES CHR banks: {}", n_chr_rom_banks);

  uint8_t cbyte1, cbyte2;
  Utils::Raw::ReadFromBinary(cbyte1, input_file);
  Utils::Raw::ReadFromBinary(cbyte2, input_file);

  uint8_t mapper = (cbyte2 & 0xF0) | (cbyte1 >> 4);
  spdlog::debug("Mapper: {}", mapper);

  uint8_t is_nes_v2 = (cbyte2 >> 2) & 0b11;
  if (is_nes_v2 != 0) {
    return make_error(std::errc::not_supported, "iNES v2.0 not supported");
  }

  Rom rom{.mapper = mapper};

  bool is_four_screen = (cbyte1 & 0b1000) != 0;
  bool is_vertical_mirroring = (cbyte1 & 0b1) != 0;

  if (is_four_screen) {
    rom.mirroring = Mirroring::FourScreen;
  } else if (is_vertical_mirroring) {
    rom.mirroring = Mirroring::Vertical;
  } else {
    rom.mirroring = Mirroring::Horizontal;
  }

  static constexpr size_t PRG_ROM_BANK_SIZE = 0x4000;
  static constexpr size_t CHR_ROM_BANK_SIZE = 0x2000;

  size_t prg_rom_size = n_prg_rom_banks * PRG_ROM_BANK_SIZE;
  size_t chr_rom_size = n_chr_rom_banks * CHR_ROM_BANK_SIZE;

  bool skip_trainer = (cbyte1 & 0b100) != 0;

  size_t prg_rom_start = 16 + (skip_trainer ? 512 : 0);
  size_t chr_rom_start = prg_rom_start + prg_rom_size;

  auto dst = prg_rom_start - input_file.tellg();
  Utils::Raw::SkipBytes(dst, input_file);

  rom.program_rom.resize(prg_rom_size);
  Utils::Raw::ReadNFromBinary(rom.program_rom, prg_rom_size, input_file);

  rom.character_rom.resize(chr_rom_size);
  Utils::Raw::ReadNFromBinary(rom.character_rom, chr_rom_size, input_file);

  return rom;
}

} // namespace BNES::HW
