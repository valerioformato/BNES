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
  TSX = 0xBA, // Transfer Stack Pointer to X
  TXS = 0x9A, // Transfer X to Stack Pointer
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
  ROR_Accumulator = 0x6A, // Rotate Right (ROR) Accumulator
  ROR_ZeroPage = 0x66,    // Rotate Right (ROR) Zero Page
  ROR_ZeroPageX = 0x76,   // Rotate Right (ROR) Zero Page,X
  ROR_Absolute = 0x6E,    // Rotate Right (ROR) Absolute
  ROR_AbsoluteX = 0x7E,   // Rotate Right (ROR) Absolute,X
  ROL_Accumulator = 0x2A, // Rotate Left (ROL) Accumulator
  ROL_ZeroPage = 0x26,    // Rotate Left (ROL) Zero Page
  ROL_ZeroPageX = 0x36,   // Rotate Left (ROL) Zero Page,X
  ROL_Absolute = 0x2E,    // Rotate Left (ROL) Absolute
  ROL_AbsoluteX = 0x3E,   // Rotate Left (ROL) Absolute,X
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
  RTI = 0x40,          // Return from Interrupt (RTI)

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
  NOP = 0xEA,           // No Operation

  // Undocumented opcodes
  // DOP (Double NOP) - No operation, reads argument but ignores it
  DOP_Immediate_80 = 0x80, // DOP #arg (Immediate)
  DOP_Immediate_82 = 0x82, // DOP #arg (Immediate)
  DOP_Immediate_89 = 0x89, // DOP #arg (Immediate)
  DOP_Immediate_C2 = 0xC2, // DOP #arg (Immediate)
  DOP_Immediate_E2 = 0xE2, // DOP #arg (Immediate)
  DOP_ZeroPage_04 = 0x04,  // DOP arg (Zero Page)
  DOP_ZeroPage_44 = 0x44,  // DOP arg (Zero Page)
  DOP_ZeroPage_64 = 0x64,  // DOP arg (Zero Page)
  DOP_ZeroPageX_14 = 0x14, // DOP arg,X (Zero Page,X)
  DOP_ZeroPageX_34 = 0x34, // DOP arg,X (Zero Page,X)
  DOP_ZeroPageX_54 = 0x54, // DOP arg,X (Zero Page,X)
  DOP_ZeroPageX_74 = 0x74, // DOP arg,X (Zero Page,X)
  DOP_ZeroPageX_D4 = 0xD4, // DOP arg,X (Zero Page,X)
  DOP_ZeroPageX_F4 = 0xF4, // DOP arg,X (Zero Page,X)

  // TOP (Triple NOP) - No operation, reads 2-byte argument but ignores it
  TOP_Absolute = 0x0C,     // TOP arg (Absolute)
  TOP_AbsoluteX_1C = 0x1C, // TOP arg,X (Absolute,X)
  TOP_AbsoluteX_3C = 0x3C, // TOP arg,X (Absolute,X)
  TOP_AbsoluteX_5C = 0x5C, // TOP arg,X (Absolute,X)
  TOP_AbsoluteX_7C = 0x7C, // TOP arg,X (Absolute,X)
  TOP_AbsoluteX_DC = 0xDC, // TOP arg,X (Absolute,X)
  TOP_AbsoluteX_FC = 0xFC, // TOP arg,X (Absolute,X)

  // NOP (Undocumented NOP variants) - No operation, implied addressing
  NOP_1A = 0x1A, // NOP (Implied)
  NOP_3A = 0x3A, // NOP (Implied)
  NOP_5A = 0x5A, // NOP (Implied)
  NOP_7A = 0x7A, // NOP (Implied)
  NOP_DA = 0xDA, // NOP (Implied)
  NOP_FA = 0xFA, // NOP (Implied)

  // LAX (Load A and X) - Load accumulator and X register with memory
  LAX_ZeroPage = 0xA7,  // LAX arg (Zero Page)
  LAX_ZeroPageY = 0xB7, // LAX arg,Y (Zero Page,Y)
  LAX_Absolute = 0xAF,  // LAX arg (Absolute)
  LAX_AbsoluteY = 0xBF, // LAX arg,Y (Absolute,Y)
  LAX_IndirectX = 0xA3, // LAX (arg,X) (Indirect,X)
  LAX_IndirectY = 0xB3, // LAX (arg),Y (Indirect,Y)

  // SAX (Store A AND X) - Store bitwise AND of A and X to memory
  SAX_ZeroPage = 0x87,  // SAX arg (Zero Page)
  SAX_ZeroPageY = 0x97, // SAX arg,Y (Zero Page,Y)
  SAX_IndirectX = 0x83, // SAX (arg,X) (Indirect,X)
  SAX_Absolute = 0x8F,  // SAX arg (Absolute)

  // SBC (Undocumented SBC variant) - Same as official SBC #immediate (0xE9)
  SBC_Immediate_EB = 0xEB, // SBC #arg (Immediate)

  // DCP (Decrement and Compare) - DEC memory then CMP with accumulator
  DCP_ZeroPage = 0xC7,  // DCP arg (Zero Page)
  DCP_ZeroPageX = 0xD7, // DCP arg,X (Zero Page,X)
  DCP_Absolute = 0xCF,  // DCP arg (Absolute)
  DCP_AbsoluteX = 0xDF, // DCP arg,X (Absolute,X)
  DCP_AbsoluteY = 0xDB, // DCP arg,Y (Absolute,Y)
  DCP_IndirectX = 0xC3, // DCP (arg,X) (Indirect,X)
  DCP_IndirectY = 0xD3, // DCP (arg),Y (Indirect,Y)

  // ISB (Increment and Subtract with Borrow) - INC memory then SBC from accumulator
  // Also known as ISC (Increment and Subtract with Carry)
  ISB_ZeroPage = 0xE7,  // ISB arg (Zero Page)
  ISB_ZeroPageX = 0xF7, // ISB arg,X (Zero Page,X)
  ISB_Absolute = 0xEF,  // ISB arg (Absolute)
  ISB_AbsoluteX = 0xFF, // ISB arg,X (Absolute,X)
  ISB_AbsoluteY = 0xFB, // ISB arg,Y (Absolute,Y)
  ISB_IndirectX = 0xE3, // ISB (arg,X) (Indirect,X)
  ISB_IndirectY = 0xF3, // ISB (arg),Y (Indirect,Y)
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
