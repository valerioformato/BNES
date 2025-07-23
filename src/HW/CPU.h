#ifndef BNES_HW_CPU_H
#define BNES_HW_CPU_H

#include "HW/OpCodes.h"
#include "common/EnumArray.h"
#include "common/Utils.h"

#include <bitset>
#include <cstdint>
#include <span>

namespace BNES::HW {
class CPU {
public:
  struct Instruction {
    OpCode opcode;                 // The opcode of the instruction
    AddressingMode mode;           // The addressing mode used by the instruction
    uint8_t cycles;                // Number of cycles the instruction takes
    uint8_t size;                  // Size of the instruction in bytes
    std::vector<uint8_t> operands; // Operands for the instruction
  };

  using Addr = uint16_t;

  enum class Register : uint8_t { A = 0, X, Y };
  enum class StatusFlag : uint8_t { Carry = 0, Zero, InterruptDisable, DecimalMode, BreakCommand, Overflow, Negative };

  static constexpr size_t STACK_MEM_SIZE = 2048;
  static constexpr size_t PROG_MEM_SIZE = 32767;

  ErrorOr<void> LoadProgram(std::vector<uint8_t> program);

  // Helper functions to inspect the state of the CPU
  [[nodiscard]] EnumArray<uint8_t, Register> Registers() const { return m_registers; };
  [[nodiscard]] uint8_t StackPointer() const { return m_stack_pointer; }
  [[nodiscard]] Addr ProgramCounter() const { return m_program_counter; }
  [[nodiscard]] std::bitset<8> StatusFlags() const { return m_status; }
  [[nodiscard]] bool TestStatusFlag(StatusFlag flag) const { return m_status.test(static_cast<size_t>(flag)); }

  [[nodiscard]] const std::array<uint8_t, PROG_MEM_SIZE> &ProgramMemory() const { return m_program_memory; }

  [[nodiscard]] Instruction DecodeInstruction(std::span<uint8_t> bytes) const;
  void RunInstruction(const Instruction &instr);

private:
  class NonMaskableInterrupt : public std::exception {};

  void SetStatusFlag(StatusFlag flag, bool value) { m_status.set(static_cast<size_t>(flag), value); }
  void ToggleStatusFlag(StatusFlag flag) { m_status.flip(static_cast<size_t>(flag)); }

  static constexpr Addr StackBaseAddress{0x0100}; // Base address for the stack
  static constexpr Addr ProgramBaseAddress{0x8000};

  EnumArray<uint8_t, Register> m_registers{}; // Array to hold CPU registers A, X, and Y
  std::bitset<8> m_status{0x00};              // Status register (flags)
  uint8_t m_stack_pointer{0xFF};              // Stack pointer initialized to 0xFF
  Addr m_program_counter{ProgramBaseAddress}; // Program counter

  std::array<uint8_t, STACK_MEM_SIZE> m_ram_memory{}; // CPU memory (2kB)
  std::array<uint8_t, PROG_MEM_SIZE> m_program_memory{};
};
} // namespace BNES::HW

#endif
