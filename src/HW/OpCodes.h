//
// Created by vformato on 7/22/25.
//

#ifndef OPCODES_H
#define OPCODES_H

#include "CPU.h"

#include <cstdint>

namespace BNES::HW {

enum class OpCode : uint8_t {
  Break = 0x00, // Break (BRK)

  // Load/Store Instructions
  LDA_Immediate = 0xA9, // Load Accumulator Immediate
  LDX_Immediate = 0xA2, // Load X Register Immediate
  LDY_Immediate = 0xA0, // Load Y Register Immediate
  LDA_ZeroPage = 0xA5,  // Load Accumulator Zero Page
  LDX_ZeroPage = 0xA6,  // Load X Register Zero Page
  LDY_ZeroPage = 0xA4,  // Load Y Register Zero Page
  LDA_ZeroPageX = 0xB5, // Load Accumulator Zero Page,X
  LDX_ZeroPageY = 0xB6, // Load X Register Zero Page,Y
  LDY_ZeroPageX = 0xB4, // Load Y Register Zero Page,X
  LDA_Absolute = 0xAD,  // Load Accumulator Absolute
  LDX_Absolute = 0xAE,  // Load X Register Absolute
  LDY_Absolute = 0xAC,  // Load Y Register Absolute
  LDA_AbsoluteX = 0xBD, // Load Accumulator Absolute,X
  LDA_AbsoluteY = 0xB9, // Load Accumulator Absolute,Y
  LDX_AbsoluteY = 0xBE, // Load X Register Absolute,Y
  LDY_AbsoluteX = 0xBC, // Load Y Register Absolute,X
  LDA_IndirectX = 0xA1, // Load Accumulator (Indirect,X)
  LDA_IndirectY = 0xB1, // Load Accumulator (Indirect),Y
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

enum class AddressingMode : uint8_t {
  Immediate = 0,
  ZeroPage = 1,
  ZeroPageX = 2,
  ZeroPageY = 3,
  Absolute = 4,
  AbsoluteX = 5,
  AbsoluteY = 6,
  IndirectX = 7,
  IndirectY = 8,
};

} // namespace BNES::HW

#endif // OPCODES_H
