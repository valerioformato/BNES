//
// Created by vformato on 7/22/25.
//

#ifndef OPCODES_H
#define OPCODES_H

#include <cstdint>

namespace BNES::HW {
enum class AddressingMode : uint8_t {
  Immediate,
  ZeroPage,
  ZeroPageX,
  Absolute,
  AbsoluteX,
  AbsoluteY,
  IndirectX,
  IndirectY,
};

enum class OpCode : uint8_t {
  Break = 0x00, // Break (BRK)

  // Load/Store Instructions
  LDA_Immediate = 0xA9, // Load Accumulator Immediate
  LDA_ZeroPage = 0xA5,  // Load Accumulator Zero Page
  LDX_Immediate = 0xA2, // Load X Register Immediate
  LDY_Immediate = 0xA0, // Load Y Register Immediate
  STA_Absolute = 0x8D,  // Store Accumulator Absolute
  STX_Absolute = 0x8E,  // Store X Register Absolute
  STY_Absolute = 0x8C,  // Store Y Register Absolute

  // Transfer Instructions
  TAX = 0xAA, // Transfer Accumulator to X

  // Increment/Decrement Instructions
  INX = 0xE8, // Increment X Register

  // Other Instructions
  NOP = 0xEA // No Operation
};
} // namespace BNES::HW

#endif // OPCODES_H
