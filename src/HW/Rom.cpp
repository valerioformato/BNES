//
// Created by vformato on 12/23/25.
//

#include "Rom.h"

#include "spdlog/sinks/stdout_color_sinks-inl.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>

namespace BNES::HW {

static size_t CalculateRomSize(uint8_t lsb, uint8_t msb_nibble, bool is_chr) {
  size_t base_unit = is_chr ? 0x2000 : 0x4000;
  if (msb_nibble < 0xF) {
    return (static_cast<size_t>(msb_nibble) << 8 | lsb) * base_unit;
  }

  uint8_t exponent = lsb & 0x3F;
  uint8_t multiplier = ((lsb >> 6) & 0x3) * 2 + 1;
  return (1ULL << exponent) * multiplier;
}

static size_t CalculateRamSize(uint8_t shift_count) {
  if (shift_count == 0) {
    return 0;
  }
  return 64ULL << shift_count;
}

ErrorOr<Rom> Rom::FromFile(std::string_view path) {
  static auto logger = spdlog::stdout_color_st("Rom::FromFile");

  std::filesystem::path p(path);
  if (!std::filesystem::exists(p)) {
    return make_error(std::errc::no_such_file_or_directory, fmt::format("File {} not found", path));
  }

  std::ifstream input_file{path.data(), std::ios::binary};
  if (!input_file.is_open()) {
    return make_error(std::errc::io_error, fmt::format("Failed to open file {}", path));
  }

  static constexpr std::array<uint8_t, 4> NES_TAG{0x4E, 0x45, 0x53, 0x1A};
  std::vector<uint8_t> header(NES_TAG.size());
  Utils::Raw::ReadNFromBinary(header, NES_TAG.size(), input_file);

  if (auto [r1, r2] = std::ranges::mismatch(NES_TAG, header); r1 != end(NES_TAG) && r2 != end(header)) {
    return make_error(std::errc::not_supported, fmt::format("Could not find NES header in file {}", path));
  }

  logger->debug("Found NES header tag in file {}", path);

  uint8_t n_prg_rom_banks = 0;
  Utils::Raw::ReadFromBinary(n_prg_rom_banks, input_file);

  uint8_t n_chr_rom_banks = 0;
  Utils::Raw::ReadFromBinary(n_chr_rom_banks, input_file);

  logger->debug("NES ROM banks: {}", n_prg_rom_banks);
  logger->debug("NES CHR banks: {}", n_chr_rom_banks);

  uint8_t cbyte1 = 0, cbyte2 = 0;
  Utils::Raw::ReadFromBinary(cbyte1, input_file);
  Utils::Raw::ReadFromBinary(cbyte2, input_file);

  uint8_t mapper_upper = (cbyte2 & 0xF0);
  uint8_t mapper_lower = (cbyte1 >> 4);

  uint8_t byte8 = 0, byte9 = 0, byte10 = 0, byte11 = 0, byte12 = 0, byte13 = 0, byte14 = 0, byte15 = 0;
  Utils::Raw::ReadFromBinary(byte8, input_file);
  Utils::Raw::ReadFromBinary(byte9, input_file);
  Utils::Raw::ReadFromBinary(byte10, input_file);
  Utils::Raw::ReadFromBinary(byte11, input_file);
  Utils::Raw::ReadFromBinary(byte12, input_file);
  Utils::Raw::ReadFromBinary(byte13, input_file);
  Utils::Raw::ReadFromBinary(byte14, input_file);
  Utils::Raw::ReadFromBinary(byte15, input_file);

  uint8_t is_nes_v2_check = (cbyte2 & 0x0C);
  bool is_nes_v2 = (is_nes_v2_check == 0x08);

  logger->debug("NES 2.0: {}", is_nes_v2);

  Rom rom{.is_nes_v2 = is_nes_v2};

  if (is_nes_v2) {
    rom.mapper = mapper_upper | mapper_lower | ((byte8 & 0x0F) << 8);
    rom.submapper = (byte8 >> 4);
    logger->debug("Mapper: {} (submapper: {})", rom.mapper, rom.submapper);

    uint8_t prg_msb = byte9 & 0x0F;
    uint8_t chr_msb = (byte9 >> 4) & 0x0F;

    rom.prg_ram_size = CalculateRamSize(byte10 & 0x0F);
    rom.prg_nvram_size = CalculateRamSize((byte10 >> 4) & 0x0F);
    rom.chr_ram_size = CalculateRamSize(byte11 & 0x0F);
    rom.chr_nvram_size = CalculateRamSize((byte11 >> 4) & 0x0F);

    rom.timing = static_cast<TimingMode>(byte12 & 0x03);
    logger->debug("Timing mode: {}", static_cast<int>(rom.timing));

    uint8_t console_bits = cbyte2 & 0x03;
    rom.console_type = static_cast<ConsoleType>(console_bits);
    logger->debug("Console type: {}", static_cast<int>(rom.console_type));

    if (rom.console_type == ConsoleType::VsSystem) {
      rom.vs_system_type = byte13;
      logger->debug("Vs. System type: {}", rom.vs_system_type);
    } else if (rom.console_type == ConsoleType::Extended) {
      rom.extended_console_type = byte13 & 0x0F;
      logger->debug("Extended console type: {}", rom.extended_console_type);
    }

    rom.misc_rom_count = byte14 & 0x03;
    rom.expansion_device = byte15 & 0x3F;

    rom.program_rom.resize(CalculateRomSize(n_prg_rom_banks, prg_msb, false));
    rom.character_rom.resize(CalculateRomSize(n_chr_rom_banks, chr_msb, true));
  } else {
    rom.mapper = mapper_upper | mapper_lower;
    logger->debug("Mapper: {}", rom.mapper);

    rom.program_rom.resize(n_prg_rom_banks * 0x4000);
    rom.character_rom.resize(n_chr_rom_banks * 0x2000);
  }

  bool is_four_screen = (cbyte1 & 0b1000) != 0;
  bool is_vertical_mirroring = (cbyte1 & 0b1) != 0;

  if (is_four_screen) {
    rom.mirroring = Mirroring::FourScreen;
  } else if (is_vertical_mirroring) {
    rom.mirroring = Mirroring::Vertical;
  } else {
    rom.mirroring = Mirroring::Horizontal;
  }

  bool skip_trainer = (cbyte1 & 0b100) != 0;

  size_t prg_rom_start = 16 + (skip_trainer ? 512 : 0);

  Utils::Raw::SkipBytes(prg_rom_start - input_file.tellg(), input_file);

  Utils::Raw::ReadNFromBinary(rom.program_rom, rom.program_rom.size(), input_file);
  Utils::Raw::ReadNFromBinary(rom.character_rom, rom.character_rom.size(), input_file);

  return rom;
}

} // namespace BNES::HW
