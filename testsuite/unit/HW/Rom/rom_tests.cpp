//
// Created by vformato on 23-Dec-25.
//

#include "HW/Rom.h"
#include "localization.h"

#include <catch2/catch_test_macros.hpp>
#include <spdlog/spdlog.h>

using namespace BNES::HW;

SCENARIO("ROM unit tests") {
  GIVEN("A simple test NES rom") {
    auto test_rom_path = fmt::format("{}/assets/roms/snake.nes", localization::PROJECT_SOURCE_DIR);

    WHEN("We load the Rom") {
      auto maybe_rom = Rom::FromFile(test_rom_path);
      THEN("It is loaded correctly") { REQUIRE(maybe_rom.has_value()); };
    }
  }
}
