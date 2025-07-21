#ifndef BNES_HW_CPU_H
#define BNES_HW_CPU_H

#include "common/EnumArray.h"

#include <bitset>
#include <cstdint>

namespace BNES::HW {
class CPU {
public:
  enum class Register : uint8_t { A = 0, X, Y };

  [[nodiscard]] EnumArray<uint8_t, Register> Registers() const { return m_registers; };
  [[nodiscard]] uint8_t StackPointer() const { return m_stack_pointer; }
  [[nodiscard]] uint16_t ProgramCounter() const { return m_program_counter; }
  [[nodiscard]] std::bitset<8> StatusFlags() const { return m_status; }

private:
  EnumArray<uint8_t, Register> m_registers{}; // Array to hold CPU registers A, X, and Y
  std::bitset<8> m_status{0x00};              // Status register (flags)
  uint8_t m_stack_pointer{0xFF};              // Stack pointer initialized to 0xFF
  uint16_t m_program_counter{0x0000};         // Program counter

  std::array<uint8_t, 2048> m_memory{}; // CPU memory (2kB)

  static constexpr uint16_t StackBaseAddress{0x0100}; // Base address for the stack
};
} // namespace BNES::HW

#endif
