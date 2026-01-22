#include "HW/CPU.h"

using namespace BNES::HW;

// ===== TransferRegisterTo =====

template <CPU::Register SRCREG, CPU::Register DSTREG>
void CPU::TransferRegisterTo<SRCREG, DSTREG>::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#TAX (or #TAY)

  cpu.m_registers[DSTREG] = cpu.m_registers[SRCREG];
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[DSTREG] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[DSTREG] & 0x80);
}

void CPU::TransferStackPointerToX::Apply(CPU &cpu) const {
  cpu.m_registers[Register::X] = cpu.m_stack_pointer;

  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::X] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::X] & 0x80);
}

void CPU::TransferXToStackPointer::Apply(CPU &cpu) const { 
  cpu.m_stack_pointer = cpu.m_registers[Register::X]; 
}

// ===== DoubleNoOperation =====

template <AddressingMode MODE> CPU::DoubleNoOperation<MODE>::DoubleNoOperation(uint16_t _value) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::Immediate) {
    this->cycles = 2;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 3;
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    this->cycles = 4;
  } else {
    std::unreachable();
  }

  value = _value;
}

// ===== Explicit template instantiations =====

// TransferRegisterTo
template struct CPU::TransferRegisterTo<CPU::Register::A, CPU::Register::X>;
template struct CPU::TransferRegisterTo<CPU::Register::A, CPU::Register::Y>;
template struct CPU::TransferRegisterTo<CPU::Register::X, CPU::Register::A>;
template struct CPU::TransferRegisterTo<CPU::Register::Y, CPU::Register::A>;

// DoubleNoOperation
template struct CPU::DoubleNoOperation<AddressingMode::Immediate>;
template struct CPU::DoubleNoOperation<AddressingMode::ZeroPage>;
template struct CPU::DoubleNoOperation<AddressingMode::ZeroPageX>;
