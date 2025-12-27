//
// Created by vformato on 12/27/25.
//

#include "HW/Bus.h"

#include <catch2/catch_test_macros.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <numeric>
#include <ranges>

using namespace BNES::HW;

SCENARIO("Bus unit tests") {
  GIVEN("A freshly initialized NES Bus") {
    Bus bus;

    WHEN("We try loading an empty program") {
      REQUIRE(bus.LoadIntoProgramRom({}).has_value());
      THEN("Program memory is empty") {
        REQUIRE(std::ranges::none_of(bus.Rom().program_rom, [](auto byte) { return byte > 0; }));
      }
    }

    WHEN("We try loading a program too big") {
      std::vector<uint8_t> program(Bus::MAX_ADDRESSABLE_ROM_ADDRESS - Bus::ROM_START_REGISTER + 1);
      std::iota(program.begin(), program.end(), 0);

      THEN("We get an error") {
        auto load_result = bus.LoadIntoProgramRom(program);
        REQUIRE(!load_result.has_value());
        REQUIRE(load_result.error().Code() == std::errc::not_enough_memory);
      }
    }
  }
}
