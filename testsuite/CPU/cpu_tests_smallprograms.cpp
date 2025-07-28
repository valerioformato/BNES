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
  using CPU::SetRegister;
  using CPU::WriteToMemory;
};

void SimpleRun(CPUMock &cpu) {
  // Run the program
  while (true) {
    std::span bytes(std::next(cpu.ProgramMemory().begin(), cpu.ProgramCounter() - 0x8000), 3ul);
    auto next_instruction = cpu.DecodeInstruction(bytes);
    try {
      cpu.RunInstruction(next_instruction);
    } catch (const CPUMock::NonMaskableInterrupt &) {
      // NMI is not handled in this test
      break;
    } catch (const std::out_of_range &e) {
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
    // Program{
    //     .expected_register_values = {{0x00, 0x03, 0x00}},
    //     .expected_status = {0b00010011},
    //     .expected_program_counter = 0x800e,
    //     .expected_memory_slices = {},
    //     .code = {0xa2, 0x08, 0xca, 0x8e, 0x00, 0x02, 0xe0, 0x03, 0xd0, 0xf8, 0x8e, 0x01, 0x02, 0x00},
    // },
};

//,
SCENARIO("6502 code execution (small test programs)") {
  GIVEN("a new CPU instance") {
    CPUMock cpu;

    WHEN("We run a test program") {
      auto program = GENERATE(from_range(programs));
      REQUIRE(cpu.LoadProgram(program.code).has_value());

      REQUIRE_NOTHROW(SimpleRun(cpu));

      REQUIRE(cpu.Registers() == program.expected_register_values);
      REQUIRE(cpu.StatusFlags() == program.expected_status);
      REQUIRE(cpu.ProgramCounter() == program.expected_program_counter);
      for (const auto &slice : program.expected_memory_slices) {
        for (size_t i = 0; i < MemorySlice::SIZE; ++i) {
          REQUIRE(cpu.ReadFromMemory(slice.start_address + i) == slice.data[i]);
        }
      }
    }
  }
}
