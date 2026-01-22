#include "HW/CPU.h"
#include <magic_enum.hpp>
#include <spdlog/fmt/fmt.h>

using namespace BNES::HW;

// ===========================================================================================
// AddWithCarry - Add with carry
// ===========================================================================================

template <AddressingMode MODE> CPU::AddWithCarry<MODE>::AddWithCarry(uint16_t _value) {
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
  } else {
    std::unreachable();
  }

  value = _value;
}

template <AddressingMode MODE> void CPU::AddWithCarry<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_add = 0;
  if constexpr (MODE == AddressingMode::Immediate) {
    value_to_add = value & 0xFF;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_add = cpu.ReadFromMemory(value & 0xFF);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_add = cpu.ReadFromMemory((value + cpu.m_registers[Register::X]) & 0xFF);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_add = cpu.ReadFromMemory(value);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_add = cpu.ReadFromMemory(value + cpu.m_registers[Register::X]);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    value_to_add = cpu.ReadFromMemory(value + cpu.m_registers[Register::Y]);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (value + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (value + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_add = cpu.ReadFromMemory(real_addr);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = value & 0xFF;
    Addr target_addr_high = (value + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_add = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]);
  } else {
    TODO(fmt::format("AddWithCarry<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  uint16_t intermediate_result = cpu.m_registers[Register::A] + value_to_add + cpu.TestStatusFlag(StatusFlag::Carry);

  uint8_t M = cpu.m_registers[Register::A];
  uint8_t N = value_to_add & 0xFF;
  uint8_t result = uint8_t(intermediate_result & 0xFF);

  cpu.m_registers[Register::A] = result;

  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::A] & 0x80);
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Carry, intermediate_result & 0x100);
  cpu.SetStatusFlagValue(StatusFlag::Overflow, ((M ^ result) & (N ^ result) & 0x80) != 0);
}

// ===========================================================================================
// SubtractWithCarry - Subtract with carry
// ===========================================================================================

template <AddressingMode MODE> CPU::SubtractWithCarry<MODE>::SubtractWithCarry(uint16_t _value, bool undoc) {
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
  } else {
    std::unreachable();
  }

  value = _value;
  undocumented = undoc;
}

template <AddressingMode MODE> void CPU::SubtractWithCarry<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_add = 0;
  if constexpr (MODE == AddressingMode::Immediate) {
    value_to_add = value & 0xFF;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_add = cpu.ReadFromMemory(value & 0xFF);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_add = cpu.ReadFromMemory((value + cpu.m_registers[Register::X]) & 0xFF);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_add = cpu.ReadFromMemory(value);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_add = cpu.ReadFromMemory(value + cpu.m_registers[Register::X]);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    value_to_add = cpu.ReadFromMemory(value + cpu.m_registers[Register::Y]);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (value + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (value + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_add = cpu.ReadFromMemory(real_addr);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = value & 0xFF;
    Addr target_addr_high = (value + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_add = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]);
  } else {
    TODO(fmt::format("SubtractWithCarry<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  uint16_t intermediate_result =
      cpu.m_registers[Register::A] - value_to_add - (1 - cpu.TestStatusFlag(StatusFlag::Carry));

  uint8_t M = cpu.m_registers[Register::A];
  uint8_t N = ~(value_to_add & 0xFF);
  uint8_t result = uint8_t(intermediate_result & 0xFF);

  cpu.m_registers[Register::A] = result;

  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::A] & 0x80);
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Carry, !(intermediate_result & 0x100));
  cpu.SetStatusFlagValue(StatusFlag::Overflow, ((M ^ result) & (N ^ result) & 0x80) != 0);
}

// ===========================================================================================
// IncrementRegister - Increment a register
// ===========================================================================================

template <CPU::Register REG> void CPU::IncrementRegister<REG>::Apply(CPU &cpu) const {
  cpu.m_registers[REG] += 1;
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[REG] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[REG] & 0x80);
}

// ===========================================================================================
// DecrementRegister - Decrement a register
// ===========================================================================================

template <CPU::Register REG> void CPU::DecrementRegister<REG>::Apply(CPU &cpu) const {
  cpu.m_registers[REG] -= 1;
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[REG] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[REG] & 0x80);
}

// ===========================================================================================
// Increment - Increment memory
// ===========================================================================================

template <AddressingMode MODE> CPU::Increment<MODE>::Increment(uint16_t addr) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 5;
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::Absolute) {
    this->size = 3;
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    this->size = 3;
    this->cycles = 7;
  } else {
    std::unreachable();
  }

  address = addr;
}

template <AddressingMode MODE> void CPU::Increment<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_write = 0;
  if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_write = cpu.ReadFromMemory(address & 0xFF) + 1;
    cpu.WriteToMemory(address & 0xFF, value_to_write);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_write = cpu.ReadFromMemory((address + cpu.m_registers[Register::X]) & 0xFF) + 1;
    cpu.WriteToMemory((address + cpu.m_registers[Register::X]) & 0xFF, value_to_write);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_write = cpu.ReadFromMemory(address) + 1;
    cpu.WriteToMemory(address, value_to_write);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_write = cpu.ReadFromMemory(address + cpu.m_registers[Register::X]) + 1;
    cpu.WriteToMemory(address + cpu.m_registers[Register::X], value_to_write);
  } else {
    TODO(fmt::format("Increment<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.SetStatusFlagValue(StatusFlag::Zero, value_to_write == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, value_to_write & 0x80);
}

// ===========================================================================================
// Decrement - Decrement memory
// ===========================================================================================

template <AddressingMode MODE> CPU::Decrement<MODE>::Decrement(uint16_t addr) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 5;
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::Absolute) {
    this->size = 3;
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    this->size = 3;
    this->cycles = 7;
  } else {
    std::unreachable();
  }

  address = addr;
}

template <AddressingMode MODE> void CPU::Decrement<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_write = 0;
  if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_write = cpu.ReadFromMemory(address & 0xFF) - 1;
    cpu.WriteToMemory(address & 0xFF, value_to_write);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_write = cpu.ReadFromMemory((address + cpu.m_registers[Register::X]) & 0xFF) - 1;
    cpu.WriteToMemory((address + cpu.m_registers[Register::X]) & 0xFF, value_to_write);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_write = cpu.ReadFromMemory(address) - 1;
    cpu.WriteToMemory(address, value_to_write);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_write = cpu.ReadFromMemory(address + cpu.m_registers[Register::X]) - 1;
    cpu.WriteToMemory(address + cpu.m_registers[Register::X], value_to_write);
  } else {
    TODO(fmt::format("Decrement<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.SetStatusFlagValue(StatusFlag::Zero, value_to_write == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, value_to_write & 0x80);
}

// ===========================================================================================
// Explicit template instantiations
// ===========================================================================================

// AddWithCarry
template struct CPU::AddWithCarry<AddressingMode::Immediate>;
template struct CPU::AddWithCarry<AddressingMode::ZeroPage>;
template struct CPU::AddWithCarry<AddressingMode::ZeroPageX>;
template struct CPU::AddWithCarry<AddressingMode::Absolute>;
template struct CPU::AddWithCarry<AddressingMode::AbsoluteX>;
template struct CPU::AddWithCarry<AddressingMode::AbsoluteY>;
template struct CPU::AddWithCarry<AddressingMode::IndirectX>;
template struct CPU::AddWithCarry<AddressingMode::IndirectY>;

// SubtractWithCarry
template struct CPU::SubtractWithCarry<AddressingMode::Immediate>;
template struct CPU::SubtractWithCarry<AddressingMode::ZeroPage>;
template struct CPU::SubtractWithCarry<AddressingMode::ZeroPageX>;
template struct CPU::SubtractWithCarry<AddressingMode::Absolute>;
template struct CPU::SubtractWithCarry<AddressingMode::AbsoluteX>;
template struct CPU::SubtractWithCarry<AddressingMode::AbsoluteY>;
template struct CPU::SubtractWithCarry<AddressingMode::IndirectX>;
template struct CPU::SubtractWithCarry<AddressingMode::IndirectY>;

// IncrementRegister
template struct CPU::IncrementRegister<CPU::Register::X>;
template struct CPU::IncrementRegister<CPU::Register::Y>;

// DecrementRegister
template struct CPU::DecrementRegister<CPU::Register::X>;
template struct CPU::DecrementRegister<CPU::Register::Y>;

// Increment
template struct CPU::Increment<AddressingMode::ZeroPage>;
template struct CPU::Increment<AddressingMode::ZeroPageX>;
template struct CPU::Increment<AddressingMode::Absolute>;
template struct CPU::Increment<AddressingMode::AbsoluteX>;

// Decrement
template struct CPU::Decrement<AddressingMode::ZeroPage>;
template struct CPU::Decrement<AddressingMode::ZeroPageX>;
template struct CPU::Decrement<AddressingMode::Absolute>;
template struct CPU::Decrement<AddressingMode::AbsoluteX>;
