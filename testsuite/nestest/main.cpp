//
// Created by vformato on 9/16/25.
//

#include "HW/CPU.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>

class NESTestCPU : public BNES::HW::CPU {
public:
  using CPU::NonMaskableInterrupt;
  using CPU::ProgramCounter;
  using CPU::ReadFromMemory;
  using CPU::SetProgramStartAddress;
  using CPU::WriteToMemory;

  NESTestCPU(BNES::HW::Bus &bus) : CPU(bus) {}

  void RunOneInstruction() {
    static constexpr auto cpu_freq = std::chrono::duration<double>(1.0f / 1790000.0f); // 1.79 MHz

    std::array<uint8_t, 3> bytes{};
    for (size_t i = 0; i < 3; ++i) {
      bytes[i] = ReadFromMemory(ProgramCounter() + i);
    }
    auto instr = DecodeInstruction(bytes);
    auto instr_disass = DisassembleInstruction(instr);
    unsigned int instr_cycles = std::visit([](DecodedInstruction &arg) { return arg.cycles; }, instr);

    auto program_counter = ProgramCounter();
    auto bytes_to_print = bytes | std::views::transform([](uint8_t byte) { return fmt::format("{:02X}", byte); });
    std::string opcodes =
        std::reduce(begin(bytes_to_print), end(bytes_to_print), std::string{}, [](auto partial, auto current) {
          return fmt::format("{}{}{}", partial, partial.size() > 0 ? " " : "", current);
        });
    last_log_line = fmt::format("{:4X}  {:<9} {}", program_counter, opcodes, instr_disass);

    RunInstruction(std::move(instr));
  }
  std::string last_log_line;
};

BNES::ErrorOr<int> nestest_main() {
  spdlog::set_level(spdlog::level::debug);

  BNES::HW::Bus bus;
  TRY(bus.LoadRom("assets/roms/nestest.nes"));

  NESTestCPU cpu{bus};
  cpu.Init();

  // Force the start in automated mode.
  // The reset vector points to a starting address that we can use once we implement a working PPU. For now, let's run
  // in "batch" mode
  cpu.SetProgramStartAddress(0xC000);

  while (true) {
    try {
      cpu.RunOneInstruction();
      fmt::println("{}", cpu.last_log_line);
    } catch (const NESTestCPU::NonMaskableInterrupt &nmi) {
      break;
    }
  }

  return 0;
}

int main() {
  auto result = nestest_main();
  if (!result) {
    spdlog::error("Error: {}", result.error().Message());
    return result.error().Code().value();
  }

  return 0;
}
