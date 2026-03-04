//
// Created by vformato on 23-Dec-25.
//

#include "HW/Rom.h"
#include "localization.h"

#include <catch2/catch_test_macros.hpp>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>

using namespace BNES::HW;

// Writes a synthetic NES 2.0 ROM to a temp file and returns its path.
// The header encodes the given PRG/CHR bank counts, timing, and mapper.
// PRG and CHR data is filled with zeros.
static std::filesystem::path WriteSyntheticNes2Rom(uint8_t prg_banks, uint8_t chr_banks, uint8_t timing,
                                                   uint16_t mapper = 0, uint8_t submapper = 0) {
  auto path = std::filesystem::temp_directory_path() /
              fmt::format("bnes_test_{}_{}_{}.nes", prg_banks, chr_banks, timing);

  // clang-format off
  // NES 2.0 header (16 bytes)
  // byte 7: bits 3:2 == 0b10 => NES 2.0 identifier (0x08)
  std::array<uint8_t, 16> header = {
    0x4E, 0x45, 0x53, 0x1A,                                    // "NES\x1A" tag
    prg_banks,                                                  // PRG ROM size LSB
    chr_banks,                                                  // CHR ROM size LSB
    0x00,                                                       // flags6
    static_cast<uint8_t>(0x08 | ((mapper & 0xF0) >> 4 << 4)),  // flags7: NES 2.0 marker
    static_cast<uint8_t>(((mapper >> 8) & 0x0F) | (submapper << 4)), // mapper MSB + submapper
    0x00,                                                       // PRG/CHR ROM size MSB nibbles
    0x00,                                                       // PRG RAM/NVRAM size
    0x00,                                                       // CHR RAM/NVRAM size
    timing,                                                     // timing mode
    0x00,                                                       // console/vs type
    0x00,                                                       // misc ROMs
    0x00,                                                       // expansion device
  };
  // clang-format on

  std::ofstream out{path, std::ios::binary};
  out.write(reinterpret_cast<const char *>(header.data()), header.size());

  size_t prg_size = prg_banks * 0x4000;
  size_t chr_size = chr_banks * 0x2000;
  std::vector<uint8_t> zeros(prg_size + chr_size, 0x00);
  out.write(reinterpret_cast<const char *>(zeros.data()), static_cast<std::streamsize>(zeros.size()));

  return path;
}

SCENARIO("ROM unit tests") {
  GIVEN("A simple test NES rom") {
    auto test_rom_path = fmt::format("{}/assets/roms/snake.nes", localization::PROJECT_SOURCE_DIR);

    WHEN("We load the Rom") {
      auto maybe_rom = Rom::FromFile(test_rom_path);
      THEN("It is loaded correctly") { REQUIRE(maybe_rom.has_value()); };
    }
  }

  GIVEN("A NES 2.0 ROM with 1 PRG bank, 1 CHR bank, PAL timing, mapper 0") {
    // timing 0x01 = PAL (Rom::TimingMode::PAL)
    auto path = WriteSyntheticNes2Rom(1, 1, 0x01);

    WHEN("We load the Rom") {
      auto maybe_rom = Rom::FromFile(path.string());
      THEN("It is parsed correctly") {
        REQUIRE(maybe_rom.has_value());
        auto &rom = maybe_rom.value();
        REQUIRE(rom.is_nes_v2 == true);
        REQUIRE(rom.mapper == 0);
        REQUIRE(rom.submapper == 0);
        REQUIRE(rom.program_rom.size() == 0x4000);
        REQUIRE(rom.character_rom.size() == 0x2000);
        REQUIRE(rom.timing == Rom::TimingMode::PAL);
        REQUIRE(rom.console_type == Rom::ConsoleType::NES);
      };
    }

    std::filesystem::remove(path);
  }

  GIVEN("A NES 2.0 ROM with 2 PRG banks, 1 CHR bank, MultiRegion timing, mapper 0") {
    // timing 0x02 = MultiRegion (Rom::TimingMode::MultiRegion)
    auto path = WriteSyntheticNes2Rom(2, 1, 0x02);

    WHEN("We load the Rom") {
      auto maybe_rom = Rom::FromFile(path.string());
      THEN("It is parsed correctly") {
        REQUIRE(maybe_rom.has_value());
        auto &rom = maybe_rom.value();
        REQUIRE(rom.is_nes_v2 == true);
        REQUIRE(rom.mapper == 0);
        REQUIRE(rom.submapper == 0);
        REQUIRE(rom.program_rom.size() == 0x8000);
        REQUIRE(rom.character_rom.size() == 0x2000);
        REQUIRE(rom.timing == Rom::TimingMode::MultiRegion);
        REQUIRE(rom.console_type == Rom::ConsoleType::NES);
      };
    }

    std::filesystem::remove(path);
  }
}
