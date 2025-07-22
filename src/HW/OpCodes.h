//
// Created by vformato on 7/22/25.
//

#ifndef OPCODES_H
#define OPCODES_H

#include <cstdint>

namespace BNES::HW {
enum class OpCode : uint8_t {
  // Load/Store Instructions
  LDA_Immediate = 0xA9, // Load Accumulator Immediate
  LDX_Immediate = 0xA2, // Load X Register Immediate
  LDY_Immediate = 0xA0, // Load Y Register Immediate
  STA_Absolute = 0x8D,  // Store Accumulator Absolute
  STX_Absolute = 0x8E,  // Store X Register Absolute
  STY_Absolute = 0x8C,  // Store Y Register Absolute

  // Other Instructions
  NOP = 0xEA // No Operation
};
}

#endif // OPCODES_H
