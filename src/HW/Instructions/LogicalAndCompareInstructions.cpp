#include "HW/CPU.h"
#include <magic_enum.hpp>
#include <spdlog/fmt/fmt.h>

using namespace BNES::HW;

// ===========================================================================================
// LogicalAND - Logical AND with accumulator
// ===========================================================================================

template <AddressingMode MODE> CPU::LogicalAND<MODE>::LogicalAND(uint16_t _value) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::Immediate) {
    this->cycles = 2;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 3;
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::Absolute || MODE == AddressingMode::AbsoluteX ||
                       MODE == AddressingMode::AbsoluteY) {
    this->size = 3;
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    this->cycles = 5;
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    this->cycles = 6;
  }

  value = _value;
}

template <AddressingMode MODE> void CPU::LogicalAND<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_and = 0;
  if constexpr (MODE == AddressingMode::Immediate) {
    value_to_and = value & 0xFF;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_and = cpu.ReadFromMemory(value & 0xFF);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_and = cpu.ReadFromMemory((value + cpu.m_registers[Register::X]) & 0xFF);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_and = cpu.ReadFromMemory(value);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_and = cpu.ReadFromMemory(value + cpu.m_registers[Register::X]);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    value_to_and = cpu.ReadFromMemory(value + cpu.m_registers[Register::Y]);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (value + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (value + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_and = cpu.ReadFromMemory(real_addr);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = value & 0xFF;
    Addr target_addr_high = (value + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_and = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]);
  } else {
    TODO(fmt::format("LogicalAND<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.m_registers[Register::A] = cpu.m_registers[Register::A] & value_to_and;

  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::A] & 0x80);
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
}

// ===========================================================================================
// ExclusiveOR - Exclusive OR with accumulator
// ===========================================================================================

template <AddressingMode MODE> CPU::ExclusiveOR<MODE>::ExclusiveOR(uint16_t addr) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::Immediate) {
    this->cycles = 2;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 3;
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::Absolute || MODE == AddressingMode::AbsoluteX ||
                       MODE == AddressingMode::AbsoluteY) {
    this->size = 3;
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    this->cycles = 5;
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    this->cycles = 6;
  }

  value = addr;
}

template <AddressingMode MODE> void CPU::ExclusiveOR<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_or{0};

  if constexpr (MODE == AddressingMode::Immediate) {
    value_to_or = value & 0xFF;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_or = cpu.ReadFromMemory(value & 0xFF);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_or = cpu.ReadFromMemory((value + cpu.m_registers[Register::X]) & 0xFF);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_or = cpu.ReadFromMemory(value);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_or = cpu.ReadFromMemory(value + cpu.m_registers[Register::X]);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    value_to_or = cpu.ReadFromMemory(value + cpu.m_registers[Register::Y]);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (value + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (value + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_or = cpu.ReadFromMemory(real_addr);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = value & 0xFF;
    Addr target_addr_high = (value + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_or = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]);
  } else {
    TODO(fmt::format("ExclusiveOR<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.m_registers[Register::A] ^= value_to_or;

  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::A] & 0x80);
}

// ===========================================================================================
// BitwiseOR - Bitwise OR with accumulator
// ===========================================================================================

template <AddressingMode MODE> CPU::BitwiseOR<MODE>::BitwiseOR(uint16_t addr) : DecodedInstruction() {
  this->size = 2;

  if constexpr (MODE == AddressingMode::Immediate) {
    this->cycles = 2;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 3;
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::Absolute || MODE == AddressingMode::AbsoluteX ||
                       MODE == AddressingMode::AbsoluteY) {
    this->size = 3;
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    this->cycles = 5;
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    this->cycles = 6;
  }

  value = addr;
}

template <AddressingMode MODE> void CPU::BitwiseOR<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_or{0};

  if constexpr (MODE == AddressingMode::Immediate) {
    value_to_or = value & 0xFF;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_or = cpu.ReadFromMemory(value & 0xFF);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_or = cpu.ReadFromMemory((value + cpu.m_registers[Register::X]) & 0xFF);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_or = cpu.ReadFromMemory(value);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_or = cpu.ReadFromMemory(value + cpu.m_registers[Register::X]);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    value_to_or = cpu.ReadFromMemory(value + cpu.m_registers[Register::Y]);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (value + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (value + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_or = cpu.ReadFromMemory(real_addr);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = value & 0xFF;
    Addr target_addr_high = (value + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_or = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]);
  } else {
    TODO(fmt::format("BitwiseOR<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.m_registers[Register::A] |= value_to_or;

  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::A] & 0x80);
}

// ===========================================================================================
// BitTest - Bit test
// ===========================================================================================

template <AddressingMode MODE> CPU::BitTest<MODE>::BitTest(uint16_t addr) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 3;
  } else if constexpr (MODE == AddressingMode::Absolute) {
    this->size = 3;
    this->cycles = 4;
  }

  address = addr;
}

template <AddressingMode MODE> void CPU::BitTest<MODE>::Apply(CPU &cpu) const {
  uint8_t value{0};
  if constexpr (MODE == AddressingMode::ZeroPage) {
    Addr addr = address & 0xFF;
    value = cpu.ReadFromMemory(addr);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value = cpu.ReadFromMemory(address);
  } else {
    TODO(fmt::format("BitTest<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  uint8_t result = cpu.m_registers[Register::A] & value;
  cpu.SetStatusFlagValue(StatusFlag::Zero, result == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, value & 0b10000000);
  cpu.SetStatusFlagValue(StatusFlag::Overflow, value & 0b01000000);
}

// ===========================================================================================
// CompareRegister - Compare register with memory
// ===========================================================================================

template <CPU::Register REG, AddressingMode MODE> CPU::CompareRegister<REG, MODE>::CompareRegister(uint16_t _value) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::Immediate) {
    this->cycles = 2;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 3;
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::Absolute || MODE == AddressingMode::AbsoluteX ||
                       MODE == AddressingMode::AbsoluteY) {
    this->size = 3;
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    this->cycles = 5;
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    this->cycles = 6;
  }

  value = _value;
}

template <CPU::Register REG, AddressingMode MODE> void CPU::CompareRegister<REG, MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_compare{0};
  if constexpr (MODE == AddressingMode::Immediate) {
    value_to_compare = value & 0xFF;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_compare = cpu.ReadFromMemory(value & 0xFF);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    Addr effective_addr = (value + cpu.m_registers[Register::X]) & 0xFF;
    value_to_compare = cpu.ReadFromMemory(effective_addr);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_compare = cpu.ReadFromMemory(value);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    Addr effective_addr = value + cpu.m_registers[Register::X];
    value_to_compare = cpu.ReadFromMemory(effective_addr);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    Addr effective_addr = value + cpu.m_registers[Register::Y];
    value_to_compare = cpu.ReadFromMemory(effective_addr);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (value + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (value + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_compare = cpu.ReadFromMemory(real_addr);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = value & 0xFF;
    Addr target_addr_high = (value + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_compare = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]);
  }

  cpu.SetStatusFlagValue(StatusFlag::Carry, cpu.m_registers[REG] >= value_to_compare);
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[REG] == value_to_compare);
  cpu.SetStatusFlagValue(StatusFlag::Negative, (cpu.m_registers[REG] - value_to_compare) & 0x80);
}

// ===========================================================================================
// ClearStatusFlag - Clear a status flag
// ===========================================================================================

template <CPU::StatusFlag FLAG> void CPU::ClearStatusFlag<FLAG>::Apply(CPU &cpu) const {
  cpu.SetStatusFlagValue(FLAG, false);
}

// ===========================================================================================
// SetStatusFlag - Set a status flag
// ===========================================================================================

template <CPU::StatusFlag FLAG> void CPU::SetStatusFlag<FLAG>::Apply(CPU &cpu) const {
  cpu.SetStatusFlagValue(FLAG, true);
}

// ===========================================================================================
// Explicit template instantiations
// ===========================================================================================

// LogicalAND
template struct CPU::LogicalAND<AddressingMode::Immediate>;
template struct CPU::LogicalAND<AddressingMode::ZeroPage>;
template struct CPU::LogicalAND<AddressingMode::ZeroPageX>;
template struct CPU::LogicalAND<AddressingMode::Absolute>;
template struct CPU::LogicalAND<AddressingMode::AbsoluteX>;
template struct CPU::LogicalAND<AddressingMode::AbsoluteY>;
template struct CPU::LogicalAND<AddressingMode::IndirectX>;
template struct CPU::LogicalAND<AddressingMode::IndirectY>;

// ExclusiveOR
template struct CPU::ExclusiveOR<AddressingMode::Immediate>;
template struct CPU::ExclusiveOR<AddressingMode::ZeroPage>;
template struct CPU::ExclusiveOR<AddressingMode::ZeroPageX>;
template struct CPU::ExclusiveOR<AddressingMode::Absolute>;
template struct CPU::ExclusiveOR<AddressingMode::AbsoluteX>;
template struct CPU::ExclusiveOR<AddressingMode::AbsoluteY>;
template struct CPU::ExclusiveOR<AddressingMode::IndirectX>;
template struct CPU::ExclusiveOR<AddressingMode::IndirectY>;

// BitwiseOR
template struct CPU::BitwiseOR<AddressingMode::Immediate>;
template struct CPU::BitwiseOR<AddressingMode::ZeroPage>;
template struct CPU::BitwiseOR<AddressingMode::ZeroPageX>;
template struct CPU::BitwiseOR<AddressingMode::Absolute>;
template struct CPU::BitwiseOR<AddressingMode::AbsoluteX>;
template struct CPU::BitwiseOR<AddressingMode::AbsoluteY>;
template struct CPU::BitwiseOR<AddressingMode::IndirectX>;
template struct CPU::BitwiseOR<AddressingMode::IndirectY>;

// BitTest
template struct CPU::BitTest<AddressingMode::ZeroPage>;
template struct CPU::BitTest<AddressingMode::Absolute>;

// CompareRegister
template struct CPU::CompareRegister<CPU::Register::A, AddressingMode::Immediate>;
template struct CPU::CompareRegister<CPU::Register::A, AddressingMode::ZeroPage>;
template struct CPU::CompareRegister<CPU::Register::A, AddressingMode::ZeroPageX>;
template struct CPU::CompareRegister<CPU::Register::A, AddressingMode::Absolute>;
template struct CPU::CompareRegister<CPU::Register::A, AddressingMode::AbsoluteX>;
template struct CPU::CompareRegister<CPU::Register::A, AddressingMode::AbsoluteY>;
template struct CPU::CompareRegister<CPU::Register::A, AddressingMode::IndirectX>;
template struct CPU::CompareRegister<CPU::Register::A, AddressingMode::IndirectY>;
template struct CPU::CompareRegister<CPU::Register::X, AddressingMode::Immediate>;
template struct CPU::CompareRegister<CPU::Register::X, AddressingMode::ZeroPage>;
template struct CPU::CompareRegister<CPU::Register::X, AddressingMode::Absolute>;
template struct CPU::CompareRegister<CPU::Register::Y, AddressingMode::Immediate>;
template struct CPU::CompareRegister<CPU::Register::Y, AddressingMode::ZeroPage>;
template struct CPU::CompareRegister<CPU::Register::Y, AddressingMode::Absolute>;

// ClearStatusFlag
template struct CPU::ClearStatusFlag<CPU::StatusFlag::Carry>;
template struct CPU::ClearStatusFlag<CPU::StatusFlag::Overflow>;
template struct CPU::ClearStatusFlag<CPU::StatusFlag::InterruptDisable>;
template struct CPU::ClearStatusFlag<CPU::StatusFlag::DecimalMode>;

// SetStatusFlag
template struct CPU::SetStatusFlag<CPU::StatusFlag::Carry>;
template struct CPU::SetStatusFlag<CPU::StatusFlag::InterruptDisable>;
template struct CPU::SetStatusFlag<CPU::StatusFlag::DecimalMode>;
