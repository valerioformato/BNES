//
// Created by vformato on 9/16/25.
//

#include "HW/CPU.h"

#include <algorithm>
#include <concepts>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <ranges>

// Concept to check if a type has an AddrMode() method
template <typename T>
concept HasAddressingMode = requires {
  { T::AddrMode() } -> std::same_as<BNES::HW::AddressingMode>;
};

class NESTestCPU : public BNES::HW::CPU {
public:
  using CPU::NonMaskableInterrupt;
  using CPU::ProgramCounter;
  using CPU::ReadFromMemory;
  using CPU::SetProgramStartAddress;
  using CPU::WriteToMemory;

  NESTestCPU(BNES::HW::Bus &bus) : CPU(bus) {}

  // Helper to get memory value suffix for non-immediate addressing modes
  std::string GetMemoryValueSuffix(const Instruction &instr) const {
    return std::visit(
        [this](const auto &inst) -> std::string {
          using InstrType = std::decay_t<decltype(inst)>;

          // Exclude control flow instructions (branches, jumps, returns)
          if constexpr (std::is_same_v<InstrType, Branch<BNES::HW::Conditional::CarrySet>> ||
                        std::is_same_v<InstrType, Branch<BNES::HW::Conditional::CarryClear>> ||
                        std::is_same_v<InstrType, Branch<BNES::HW::Conditional::Equal>> ||
                        std::is_same_v<InstrType, Branch<BNES::HW::Conditional::NotEqual>> ||
                        std::is_same_v<InstrType, Branch<BNES::HW::Conditional::Minus>> ||
                        std::is_same_v<InstrType, Branch<BNES::HW::Conditional::Positive>> ||
                        std::is_same_v<InstrType, Branch<BNES::HW::Conditional::OverflowSet>> ||
                        std::is_same_v<InstrType, Branch<BNES::HW::Conditional::OverflowClear>> ||
                        std::is_same_v<InstrType, Jump<BNES::HW::AddressingMode::Absolute>> ||
                        std::is_same_v<InstrType, JumpToSubroutine> ||
                        std::is_same_v<InstrType, ReturnFromSubroutine> ||
                        std::is_same_v<InstrType, ReturnFromInterrupt>) {
            return "";
          }

          // Special handling for JMP Indirect to show target address
          if constexpr (std::is_same_v<InstrType, Jump<BNES::HW::AddressingMode::Indirect>>) {
            // Format: = TARGET
            // Example: JMP ($0200) = DB7E
            // Note: 6502 bug - if pointer is at page boundary (e.g. $02FF),
            // reads LSB from $02FF but MSB from $0200 (not $0300)
            uint16_t ptr_addr = inst.address;
            uint8_t low_byte = ReadFromMemory(ptr_addr);
            uint8_t high_byte;
            if ((ptr_addr & 0xFF) == 0xFF) {
              // Page boundary bug: MSB wraps within the same page
              high_byte = ReadFromMemory(ptr_addr & 0xFF00);
            } else {
              high_byte = ReadFromMemory(ptr_addr + 1);
            }
            uint16_t target_addr = (high_byte << 8) | low_byte;
            return fmt::format(" = {:04X}", target_addr);
          }

          if constexpr (HasAddressingMode<InstrType>) {
            // Get the base address/value from the instruction
            uint16_t base_value;
            if constexpr (requires { inst.address; }) {
              base_value = inst.address;
            } else if constexpr (requires { inst.value; }) {
              base_value = inst.value;
            } else {
              return "";
            }

            // Special handling for IndirectX (Indexed Indirect)
            if constexpr (InstrType::AddrMode() == BNES::HW::AddressingMode::IndirectX) {
              // Format: @ ZP = TARGET = VALUE
              // Example: @ 80 = 0200 = 5A
              uint8_t zp_base = base_value;
              uint8_t zp_addr = (zp_base + Registers()[Register::X]) & 0xFF;
              uint8_t low_byte = ReadFromMemory(zp_addr);
              uint8_t high_byte = ReadFromMemory((zp_addr + 1) & 0xFF);
              uint16_t target_addr = (high_byte << 8) | low_byte;
              uint8_t mem_value = ReadFromMemory(target_addr);
              return fmt::format(" @ {:02X} = {:04X} = {:02X}", zp_addr, target_addr, mem_value);
            }

            // Special handling for IndirectY (Indirect Indexed)
            if constexpr (InstrType::AddrMode() == BNES::HW::AddressingMode::IndirectY) {
              // Format: = BASE @ TARGET = VALUE
              // Example: = 0200 @ 0205 = 5A
              uint8_t zp_addr = base_value;
              uint8_t low_byte = ReadFromMemory(zp_addr);
              uint8_t high_byte = ReadFromMemory((zp_addr + 1) & 0xFF);
              uint16_t base_addr = (high_byte << 8) | low_byte;
              uint16_t target_addr = base_addr + Registers()[Register::Y];
              uint8_t mem_value = ReadFromMemory(target_addr);
              return fmt::format(" = {:04X} @ {:04X} = {:02X}", base_addr, target_addr, mem_value);
            }

            // Special handling for AbsoluteX (Absolute Indexed with X)
            if constexpr (InstrType::AddrMode() == BNES::HW::AddressingMode::AbsoluteX) {
              // Format: @ TARGET = VALUE
              // Example: LDA $0300,X @ 0365 = 89
              uint16_t target_addr = base_value + Registers()[Register::X];
              uint8_t mem_value = ReadFromMemory(target_addr);
              return fmt::format(" @ {:04X} = {:02X}", target_addr, mem_value);
            }

            // Special handling for AbsoluteY (Absolute Indexed with Y)
            if constexpr (InstrType::AddrMode() == BNES::HW::AddressingMode::AbsoluteY) {
              // Format: @ TARGET = VALUE
              // Example: LDA $0300,Y @ 0365 = 89
              uint16_t target_addr = base_value + Registers()[Register::Y];
              uint8_t mem_value = ReadFromMemory(target_addr);
              return fmt::format(" @ {:04X} = {:02X}", target_addr, mem_value);
            }

            // Special handling for ZeroPageX (Zero Page Indexed with X)
            if constexpr (InstrType::AddrMode() == BNES::HW::AddressingMode::ZeroPageX) {
              // Format: @ ZP = VALUE
              // Example: LDA $80,X @ 85 = AA
              uint8_t target_addr = (base_value + Registers()[Register::X]) & 0xFF;
              uint8_t mem_value = ReadFromMemory(target_addr);
              return fmt::format(" @ {:02X} = {:02X}", target_addr, mem_value);
            }

            // Special handling for ZeroPageY (Zero Page Indexed with Y)
            if constexpr (InstrType::AddrMode() == BNES::HW::AddressingMode::ZeroPageY) {
              // Format: @ ZP = VALUE
              // Example: LDX $80,Y @ 85 = AA
              uint8_t target_addr = (base_value + Registers()[Register::Y]) & 0xFF;
              uint8_t mem_value = ReadFromMemory(target_addr);
              return fmt::format(" @ {:02X} = {:02X}", target_addr, mem_value);
            }

            // Only show memory value for non-immediate, non-accumulator modes
            if constexpr (InstrType::AddrMode() != BNES::HW::AddressingMode::Immediate &&
                          InstrType::AddrMode() != BNES::HW::AddressingMode::Accumulator) {
              uint8_t mem_value = ReadFromMemory(base_value);
              return fmt::format(" = {:02X}", mem_value);
            }
          }
          return "";
        },
        instr);
  }

  void RunOneInstruction() {
    static constexpr auto cpu_freq = std::chrono::duration<double>(1.0f / 1790000.0f); // 1.79 MHz

    std::array<uint8_t, 3> bytes{};
    for (size_t i = 0; i < 3; ++i) {
      bytes[i] = ReadFromMemory(ProgramCounter() + i);
    }
    auto instr = DecodeInstruction(bytes);
    auto instr_disass = DisassembleInstruction(instr);
    unsigned int instr_cycles = std::visit([](DecodedInstruction &arg) { return arg.cycles; }, instr);
    unsigned int instr_size = std::visit([](DecodedInstruction &arg) { return arg.size; }, instr);
    auto reg_values =
        fmt::format("A:{:02X} X:{:02X} Y:{:02X} P:{:02X} SP:{:02X}", Registers()[Register::A], Registers()[Register::X],
                    Registers()[Register::Y], StatusFlags().to_ulong(), StackPointer());

    auto program_counter = ProgramCounter();
    auto bytes_to_print = bytes | std::views::take(instr_size) |
                          std::views::transform([](uint8_t byte) { return fmt::format("{:02X}", byte); });
    std::string opcodes =
        std::reduce(begin(bytes_to_print), end(bytes_to_print), std::string{}, [](auto partial, auto current) {
          return fmt::format("{}{}{}", partial, partial.size() > 0 ? " " : "", current);
        });

    auto mem_suffix = GetMemoryValueSuffix(instr);
    // Undocumented opcodes (starting with *) need one less space for alignment
    int opcode_width = instr_disass[0] == '*' ? 8 : 9;
    last_log_line = fmt::format("{:04X}  {:<{}} {}{}", program_counter, opcodes, opcode_width, instr_disass, mem_suffix);
    auto tmp_width = last_log_line.size();
    last_log_line += fmt::format("{:<{}}{}", "", 48 - tmp_width, reg_values);

    RunInstruction(std::move(instr));
  }
  std::string last_log_line;
};

BNES::ErrorOr<int> nestest_main() {
  spdlog::set_level(spdlog::level::debug);

  BNES::HW::Bus bus;
  TRY(bus.LoadRom("assets/roms/nestest.nes"));

  // Let's open the reference nestest dump log and keep it in memory so we can test our own execution to the reference
  // one
  std::vector<std::string> nestest_log;
  std::filesystem::path nestest_log_path("testsuite/nestest/nestest.log");
  if (std::filesystem::exists(nestest_log_path)) {
    std::ifstream log_file(nestest_log_path);
    while (!log_file.eof()) {
      std::string line;
      std::getline(log_file, line);
      nestest_log.push_back(line);
    }
    spdlog::info("Loaded reference nestest log with {} lines", nestest_log.size());
  } else {
    return BNES::make_error(std::errc::no_such_file_or_directory, "nestest.log not found");
  }

  NESTestCPU cpu{bus};
  cpu.Init();

  // Force the start in automated mode.
  // The reset vector points to a starting address that we can use once we implement a working PPU. For now, let's run
  // in "batch" mode
  cpu.SetProgramStartAddress(0xC000);

  auto nestest_log_it = nestest_log.begin();

  unsigned int i_line = 0;
  while (true) {
    try {
      cpu.RunOneInstruction();

      if (cpu.last_log_line != nestest_log_it->substr(0, cpu.last_log_line.size())) {
        spdlog::error("Log mismatch at line {}:\n {} \n {}", std::distance(nestest_log.begin(), nestest_log_it),
                      cpu.last_log_line, *nestest_log_it);

        break;
      }

      spdlog::debug("{} - {}", ++i_line, cpu.last_log_line);

      ++nestest_log_it;
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
