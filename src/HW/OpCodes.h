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
  STA_ZeroPage = 0x85,  // Store Accumulator Zero Page
  STX_ZeroPage = 0x86,  // Store X Register Zero Page
  STY_ZeroPage = 0x84,  // Store Y Register Zero Page
  STA_ZeroPageX = 0x95, // Store Accumulator Zero Page,X
  STX_ZeroPageY = 0x96, // Store X Register Zero Page,Y
  STY_ZeroPageX = 0x94, // Store Y Register Zero Page,X
  STA_Absolute = 0x8D,  // Store Accumulator Absolute
  STX_Absolute = 0x8E,  // Store X Register Absolute
  STY_Absolute = 0x8C,  // Store Y Register Absolute
  STA_AbsoluteX = 0x9D, // Store Accumulator Absolute,X
  STA_AbsoluteY = 0x99, // Store Accumulator Absolute,Y
  STA_IndirectX = 0x81, // Store Accumulator (Indirect,X)
  STA_IndirectY = 0x91, // Store Accumulator (Indirect),Y

  // Transfer Instructions
  TAX = 0xAA, // Transfer Accumulator to X
  TAY = 0xA8, // Transfer Accumulator to Y
  TXA = 0x8A, // Transfer X to Accumulator
  TYA = 0x98, // Transfer Y to Accumulator

  // Stack Instructions
  PHA = 0x48, // Push Accumulator onto Stack
  PLA = 0x68, // Pull Accumulator from Stack
  PHP = 0x08, // Push Processor status onto stack
  PLP = 0x28, // Pull Processor status onto stack

  // Math instructions
  ADC_Immediate = 0x69,   // Add with Carry (ADC) Immediate
  ADC_ZeroPage = 0x65,    // Add with Carry (ADC) Zero Page
  ADC_ZeroPageX = 0x75,   // Add with Carry (ADC) Zero Page,X
  ADC_Absolute = 0x6D,    // Add with Carry (ADC) Absolute
  ADC_AbsoluteX = 0x7D,   // Add with Carry (ADC) Absolute,X
  ADC_AbsoluteY = 0x79,   // Add with Carry (ADC) Absolute,Y
  ADC_IndirectX = 0x61,   // Add with Carry (ADC) (Indirect,X)
  ADC_IndirectY = 0x71,   // Add with Carry (ADC) (Indirect),Y
  AND_Immediate = 0x29,   // Logical AND (AND) Immediate
  AND_ZeroPage = 0x25,    // Logical AND (AND) Zero Page
  AND_ZeroPageX = 0x35,   // Logical AND (AND) Zero Page,X
  AND_Absolute = 0x2D,    // Logical AND (AND) Absolute
  AND_AbsoluteX = 0x3D,   // Logical AND (AND) Absolute,X
  AND_AbsoluteY = 0x39,   // Logical AND (AND) Absolute,Y
  AND_IndirectX = 0x21,   // Logical AND (AND) (Indirect,X)
  AND_IndirectY = 0x31,   // Logical AND (AND) (Indirect),Y
  ASL_Accumulator = 0x0A, // Arithmetic Shift Left (ASL) Accumulator
  ASL_ZeroPage = 0x06,    // Arithmetic Shift Left (ASL) Zero Page
  ASL_ZeroPageX = 0x16,   // Arithmetic Shift Left (ASL) Zero Page,X
  ASL_Absolute = 0x0E,    // Arithmetic Shift Left (ASL) Absolute
  ASL_AbsoluteX = 0x1E,   // Arithmetic Shift Left (ASL) Absolute,X
  LSR_Accumulator = 0x4A, // Logical Shift Right (LSR) Accumulator
  LSR_ZeroPage = 0x46,    // Logical Shift Right (LSR) Zero Page
  LSR_ZeroPageX = 0x56,   // Logical Shift Right (LSR) Zero Page,X
  LSR_Absolute = 0x04E,   // Logical Shift Right (LSR) Absolute
  LSR_AbsoluteX = 0x5E,   // Logical Shift Right (LSR) Absolute,X
  INC_ZeroPage = 0xE6,    // Increment Memory (INC) Zero Page
  INC_ZeroPageX = 0xF6,   // Increment Memory (INC) Zero Page,X
  INC_Absolute = 0xEE,    // Increment Memory (INC) Absolute
  INC_AbsoluteX = 0xFE,   // Increment Memory (INC) Absolute,X
  INX = 0xE8,             // Increment X Register
  INY = 0xC8,             // Increment X Register
  DEC_ZeroPage = 0xC6,    // Decrement Memory (DEC) Zero Page
  DEC_ZeroPageX = 0xD6,   // Decrement Memory (DEC) Zero Page,X
  DEC_Absolute = 0xCE,    // Decrement Memory (DEC) Absolute
  DEC_AbsoluteX = 0xDE,   // Decrement Memory (DEC) Absolute,X
  DEX = 0xCA,             // Increment X Register
  DEY = 0x88,             // Increment X Register
  EOR_Immediate = 0x49,   // Exclusive OR (EOR) Immediate
  EOR_ZeroPage = 0x45,    // Exclusive OR (EOR) Zero Page
  EOR_ZeroPageX = 0x55,   // Exclusive OR (EOR) Zero Page,X
  EOR_Absolute = 0x4D,    // Exclusive OR (EOR) Absolute
  EOR_AbsoluteX = 0x5D,   // Exclusive OR (EOR) Absolute,X
  EOR_AbsoluteY = 0x59,   // Exclusive OR (EOR) Absolute,Y
  EOR_IndirectX = 0x41,   // Exclusive OR (EOR) (Indirect,X)
  EOR_IndirectY = 0x51,   // Exclusive OR (EOR) (Indirect),Y
  SBC_Immediate = 0xE9,   // Subtract with Carry (SBC) Immediate
  SBC_ZeroPage = 0xE5,    // Subtract with Carry (SBC) Zero Page
  SBC_ZeroPageX = 0xF5,   // Subtract with Carry (SBC) Zero Page,X
  SBC_Absolute = 0xED,    // Subtract with Carry (SBC) Absolute
  SBC_AbsoluteX = 0xFD,   // Subtract with Carry (SBC) Absolute,X
  SBC_AbsoluteY = 0xF9,   // Subtract with Carry (SBC) Absolute,Y
  SBC_IndirectX = 0xE1,   // Subtract with Carry (SBC) (Indirect,X)
  SBC_IndirectY = 0xF1,   // Subtract with Carry (SBC) (Indirect),Y
  ORA_Immediate = 0x09,   // Bitwise OR with accumulator (ORA) Immediate
  ORA_ZeroPage = 0x05,    // Bitwise OR with accumulator (ORA) Zero Page
  ORA_ZeroPageX = 0x15,   // Bitwise OR with accumulator (ORA) Zero Page,X
  ORA_Absolute = 0x0D,    // Bitwise OR with accumulator (ORA) Absolute
  ORA_AbsoluteX = 0x1D,   // Bitwise OR with accumulator (ORA) Absolute,X
  ORA_AbsoluteY = 0x19,   // Bitwise OR with accumulator (ORA) Absolute,Y
  ORA_IndirectX = 0x01,   // Bitwise OR with accumulator (ORA) (Indirect,X)
  ORA_IndirectY = 0x11,   // Bitwise OR with accumulator (ORA) (Immediate),Y

  // Branch instructions
  BEQ = 0xF0,          // Branch if Equal (BEQ)
  BNE = 0xD0,          // Branch if Not Equal (BNE)
  BCC = 0x90,          // Branch if Carry Clear (BCC)
  BCS = 0xB0,          // Branch if Carry Set (BCS)
  BMI = 0x30,          // Branch if Minus (BMI)
  BPL = 0x10,          // Branch if Positive (BPL)
  BVC = 0x50,          // Branch if Overflow Clear (BVC)
  BVS = 0x70,          // Branch if Overflow Set (BVS)
  JMP_Absolute = 0x4C, // Jump Absolute (JMP)
  JMP_Indirect = 0x6C, // Jump Indirect (JMP)
  JSR = 0x20,          // Jump to Subroutine (JSR)
  RTS = 0x60,          // Return from Subroutine (RTS)

  // Other Instructions
  BIT_ZeroPage = 0x24,  // Bit Test Zero Page
  BIT_Absolute = 0x2C,  // Bit Test Absolute
  CLC = 0x18,           // Clear Carry Flag
  CLD = 0xD8,           // Clear Decimal Mode
  CLI = 0x58,           // Clear Interrupt Disable
  CLV = 0xB8,           // Clear Overflow Flag
  CPX_Immediate = 0xE0, // Compare X Register Immediate
  CPX_ZeroPage = 0xE4,  // Compare X Register Zero Page
  CPX_Absolute = 0xEC,  // Compare X Register Absolute
  CPY_Immediate = 0xC0, // Compare Y Register Immediate
  CPY_ZeroPage = 0xC4,  // Compare Y Register Zero Page
  CPY_Absolute = 0xCC,  // Compare Y Register Absolute
  CMP_Immediate = 0xC9, // Compare Accumulator Immediate
  CMP_ZeroPage = 0xC5,  // Compare Accumulator Zero Page
  CMP_ZeroPageX = 0xD5, // Compare Accumulator Zero Page,X
  CMP_Absolute = 0xCD,  // Compare Accumulator Absolute
  CMP_AbsoluteX = 0xDD, // Compare Accumulator Absolute,X
  CMP_AbsoluteY = 0xD9, // Compare Accumulator Absolute,Y
  CMP_IndirectX = 0xC1, // Compare Accumulator (Indirect,X)
  CMP_IndirectY = 0xD1, // Compare Accumulator (Indirect),Y
  SEC = 0x38,           // Set Carry Flag
  SED = 0xF8,           // Set Decimal Flag
  SEI = 0x78,           // Set Interrupt Disable
  NOP = 0xEA            // No Operation
};

enum class AddressingMode : uint8_t {
  Immediate = 0,
  ZeroPage,
  ZeroPageX,
  ZeroPageY,
  Absolute,
  AbsoluteX,
  AbsoluteY,
  Indirect,
  IndirectX,
  IndirectY,
  Accumulator,
};

enum class Conditional : uint8_t {
  Equal = 0,
  NotEqual,
  CarrySet,
  CarryClear,
  Minus,
  Positive,
  OverflowClear,
  OverflowSet,

};

} // namespace BNES::HW

#endif // OPCODES_H
