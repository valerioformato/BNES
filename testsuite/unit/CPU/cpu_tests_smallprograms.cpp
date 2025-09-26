#include "HW/CPU.h"
#include "catch2/generators/catch_generators.hpp"
#include "catch2/generators/catch_generators_range.hpp"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <numeric>

using namespace BNES::HW;

class CPUMock : public CPU {
  // This mock class is used to expose private methods for testing purposes.
public:
  using CPU::NonMaskableInterrupt;
  using CPU::ProgramMemory;
  using CPU::ReadFromMemory;
  using CPU::SetProgramStartAddress;
  using CPU::SetRegister;
  using CPU::WriteToMemory;
};

void SimpleRun(CPUMock &cpu) {
  while (true) {
    std::span bytes(std::next(cpu.ProgramMemory().begin(), cpu.ProgramCounter() - 0x8000), 3ul);
    try {
      cpu.RunInstruction(cpu.DecodeInstruction(bytes));
    } catch (const CPUMock::NonMaskableInterrupt &) {
      // NMI is not handled in this test
      break;
    } catch ([[maybe_unused]] const std::out_of_range &e) {
      // Out of range access, we stop the execution
      break;
    }
  }
}

struct MemorySlice {
  static constexpr uint16_t SIZE = 0x10;

  uint16_t start_address = 0x00;
  std::array<uint8_t, SIZE> data{};
};

struct Program {
  BNES::EnumArray<uint8_t, CPU::Register> expected_register_values{};
  std::bitset<8> expected_status{0x00}; // Status register (flags)
  CPU::Addr expected_program_counter{0x8000};
  std::vector<MemorySlice> expected_memory_slices{};
  std::vector<uint8_t> code{};
};

std::array programs = {
    // Example 01 at https://skilldrick.github.io/easy6502
    Program{
        .expected_register_values = {{0x08, 0x00, 0x00}},
        .expected_status = {0b00000000},
        .expected_program_counter = 0x8010,
        .expected_memory_slices = {MemorySlice{
            .start_address = 0x200,
            .data = {{0x01, 0x05, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
        }},
        .code = {0xa9, 0x01, 0x8d, 0x00, 0x02, 0xa9, 0x05, 0x8d, 0x01, 0x02, 0xa9, 0x08, 0x8d, 0x02, 0x02},
    },
    // Example 02 at https://skilldrick.github.io/easy6502
    Program{
        .expected_register_values = {{0x84, 0xc1, 0x00}},
        .expected_status = {0b10000001},
        .expected_program_counter = 0x8007,
        .expected_memory_slices = {},
        .code = {0xa9, 0xc0, 0xaa, 0xe8, 0x69, 0xc4, 0x00},
    },
    // Example 03 at https://skilldrick.github.io/easy6502
    Program{
        .expected_register_values = {{0x00, 0x00, 0x00}},
        .expected_status = {0b01000011},
        .expected_program_counter = 0x8007,
        .expected_memory_slices = {},
        .code = {0xa9, 0x80, 0x85, 0x01, 0x65, 0x01},
    },
    // Example 04 at https://skilldrick.github.io/easy6502
    Program{
        .expected_register_values = {{0x00, 0x03, 0x00}},
        .expected_status = {0b00000011},
        .expected_program_counter = 0x800e,
        .expected_memory_slices = {},
        .code = {0xa2, 0x08, 0xca, 0x8e, 0x00, 0x02, 0xe0, 0x03, 0xd0, 0xf8, 0x8e, 0x01, 0x02, 0x00},
    },
    // Example 06 at https://skilldrick.github.io/easy6502
    Program{
        .expected_register_values = {{0xcc, 0x00, 0x00}},
        .expected_status = {0b10000000},
        .expected_program_counter = 0xcc02,
        .expected_memory_slices = {},
        .code = {0xa9, 0x01, 0x85, 0xf0, 0xa9, 0xcc, 0x85, 0xf1, 0x6c, 0xf0, 0x00},
    },
    // Example 07 at https://skilldrick.github.io/easy6502
    Program{
        .expected_register_values = {{0x0a, 0x01, 0x0a}},
        .expected_status = {0b00000000},
        .expected_program_counter = 0x8012,
        .expected_memory_slices = {},
        .code = {0xa2, 0x01, 0xa9, 0x05, 0x85, 0x01, 0xa9, 0x07, 0x85, 0x02, 0xa0, 0x0a, 0x8c, 0x05, 0x07, 0xa1, 0x00},
    },
    // Example 08 at https://skilldrick.github.io/easy6502
    Program{
        .expected_register_values = {{0x0a, 0x0a, 0x01}},
        .expected_status = {0b00000000},
        .expected_program_counter = 0x8012,
        .expected_memory_slices = {},
        .code = {0xa0, 0x01, 0xa9, 0x03, 0x85, 0x01, 0xa9, 0x07, 0x85, 0x02, 0xa2, 0x0a, 0x8e, 0x04, 0x07, 0xb1, 0x01},
    },
    // Example 09 at https://skilldrick.github.io/easy6502
    Program{
        .expected_register_values = {{0x00, 0x10, 0x20}},
        .expected_status = {0b00000011},
        .expected_program_counter = 0x8019,
        .expected_memory_slices =
            {
                MemorySlice{
                    .start_address = 0x200,
                    .data = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
                             0x0f},
                },
                MemorySlice{
                    .start_address = 0x210,
                    .data = {0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
                             0x00},
                },
            },
        .code = {0xa2, 0x00, 0xa0, 0x00, 0x8a, 0x99, 0x00, 0x02, 0x48, 0xe8, 0xc8, 0xc0,
                 0x10, 0xd0, 0xf5, 0x68, 0x99, 0x00, 0x02, 0xc8, 0xc0, 0x20, 0xd0, 0xf7},
    },
};

// 0xa2,0x00,0xa0,0x00,0x8a,0x99,0x00,0x02,0x48,0xe8,0xc8,0xc0,0x10,0xd0,0xf5,0x68,0x99,0x00,0x02,0xc8,0xc0,0x20,0xd0,0xf7

SCENARIO("6502 code execution (small test programs)") {
  GIVEN("a new CPU instance") {
    CPUMock cpu;

    WHEN("We run a test program") {
      auto program = GENERATE(from_range(programs));

      REQUIRE(cpu.LoadProgram(program.code).has_value());
      cpu.SetProgramStartAddress(0x8000);

      cpu.Init();

      REQUIRE_NOTHROW(SimpleRun(cpu));

      REQUIRE(cpu.Registers() == program.expected_register_values);
      REQUIRE(cpu.StatusFlags() == program.expected_status);
      REQUIRE(cpu.ProgramCounter() == program.expected_program_counter);
      for (const auto &[start_address, data] : program.expected_memory_slices) {
        for (uint16_t i = 0; i < MemorySlice::SIZE; ++i) {
          REQUIRE(cpu.ReadFromMemory(start_address + i) == data[i]);
        }
      }
    }
  }
}
