#include "HW/CPU.h"
#include <magic_enum.hpp>
#include <spdlog/fmt/fmt.h>

using namespace BNES::HW;

// ===========================================================================================
// ShiftLeft - Arithmetic shift left
// ===========================================================================================

template <AddressingMode MODE> CPU::ShiftLeft<MODE>::ShiftLeft(uint16_t _value) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::Accumulator) {
    this->size = 1;
    this->cycles = 2;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
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

  address = _value;
}

template <AddressingMode MODE> void CPU::ShiftLeft<MODE>::Apply(CPU &cpu) const {
  uint16_t value_to_shift = 0;
  if constexpr (MODE == AddressingMode::Accumulator) {
    value_to_shift = cpu.m_registers[Register::A];
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_shift = cpu.ReadFromMemory(address & 0xFF);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_shift = cpu.ReadFromMemory((address + cpu.m_registers[Register::X]) & 0xFF);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_shift = cpu.ReadFromMemory(address);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_shift = cpu.ReadFromMemory(address + cpu.m_registers[Register::X]);
  } else {
    TODO(fmt::format("ShiftLeft<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  value_to_shift = value_to_shift << 1;

  uint8_t value_to_store = value_to_shift & 0xFF;

  if constexpr (MODE == AddressingMode::Accumulator) {
    cpu.m_registers[Register::A] = value_to_store;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    cpu.WriteToMemory(address & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    cpu.WriteToMemory((address + cpu.m_registers[Register::X]) & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    cpu.WriteToMemory(address, value_to_store);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    cpu.WriteToMemory(address + cpu.m_registers[Register::X], value_to_store);
  } else {
    TODO(fmt::format("ShiftLeft<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.SetStatusFlagValue(StatusFlag::Negative, value_to_store & 0x80);
  cpu.SetStatusFlagValue(StatusFlag::Zero, value_to_store == 0);
  cpu.SetStatusFlagValue(StatusFlag::Carry, value_to_shift & 0x100);
}

// ===========================================================================================
// ShiftRight - Logical shift right
// ===========================================================================================

template <AddressingMode MODE> CPU::ShiftRight<MODE>::ShiftRight(uint16_t _value) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::Accumulator) {
    this->size = 1;
    this->cycles = 2;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
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

  address = _value;
}

template <AddressingMode MODE> void CPU::ShiftRight<MODE>::Apply(CPU &cpu) const {
  uint16_t value_to_shift = 0;
  if constexpr (MODE == AddressingMode::Accumulator) {
    value_to_shift = cpu.m_registers[Register::A];
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_shift = cpu.ReadFromMemory(address & 0xFF);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_shift = cpu.ReadFromMemory((address + cpu.m_registers[Register::X]) & 0xFF);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_shift = cpu.ReadFromMemory(address);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_shift = cpu.ReadFromMemory(address + cpu.m_registers[Register::X]);
  } else {
    TODO(fmt::format("ShiftRight<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  uint8_t value_to_store = (value_to_shift >> 1) & 0xFF;

  if constexpr (MODE == AddressingMode::Accumulator) {
    cpu.m_registers[Register::A] = value_to_store;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    cpu.WriteToMemory(address & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    cpu.WriteToMemory((address + cpu.m_registers[Register::X]) & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    cpu.WriteToMemory(address, value_to_store);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    cpu.WriteToMemory(address + cpu.m_registers[Register::X], value_to_store);
  } else {
    TODO(fmt::format("ShiftRight<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.SetStatusFlagValue(StatusFlag::Negative, value_to_store & 0x80);
  cpu.SetStatusFlagValue(StatusFlag::Zero, value_to_store == 0);
  cpu.SetStatusFlagValue(StatusFlag::Carry, value_to_shift & 0x1);
}

// ===========================================================================================
// RotateRight - Rotate right through carry
// ===========================================================================================

template <AddressingMode MODE> CPU::RotateRight<MODE>::RotateRight(uint16_t _value) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::Accumulator) {
    this->size = 1;
    this->cycles = 2;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
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

  address = _value;
}

template <AddressingMode MODE> void CPU::RotateRight<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_rotate = 0;
  if constexpr (MODE == AddressingMode::Accumulator) {
    value_to_rotate = cpu.m_registers[Register::A];
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_rotate = cpu.ReadFromMemory(address & 0xFF);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_rotate = cpu.ReadFromMemory((address + cpu.m_registers[Register::X]) & 0xFF);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_rotate = cpu.ReadFromMemory(address);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_rotate = cpu.ReadFromMemory(address + cpu.m_registers[Register::X]);
  } else {
    TODO(fmt::format("RotateRight<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  bool old_carry = cpu.TestStatusFlag(StatusFlag::Carry);
  bool new_carry = value_to_rotate & 0x1;
  uint8_t value_to_store = (value_to_rotate >> 1) | (old_carry ? 0x80 : 0x00);

  if constexpr (MODE == AddressingMode::Accumulator) {
    cpu.m_registers[Register::A] = value_to_store;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    cpu.WriteToMemory(address & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    cpu.WriteToMemory((address + cpu.m_registers[Register::X]) & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    cpu.WriteToMemory(address, value_to_store);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    cpu.WriteToMemory(address + cpu.m_registers[Register::X], value_to_store);
  } else {
    TODO(fmt::format("RotateRight<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.SetStatusFlagValue(StatusFlag::Negative, value_to_store & 0x80);
  cpu.SetStatusFlagValue(StatusFlag::Zero, value_to_store == 0);
  cpu.SetStatusFlagValue(StatusFlag::Carry, new_carry);
}

// ===========================================================================================
// RotateLeft - Rotate left through carry
// ===========================================================================================

template <AddressingMode MODE> CPU::RotateLeft<MODE>::RotateLeft(uint16_t _value) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::Accumulator) {
    this->size = 1;
    this->cycles = 2;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
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

  address = _value;
}

template <AddressingMode MODE> void CPU::RotateLeft<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_rotate = 0;
  if constexpr (MODE == AddressingMode::Accumulator) {
    value_to_rotate = cpu.m_registers[Register::A];
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_rotate = cpu.ReadFromMemory(address & 0xFF);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_rotate = cpu.ReadFromMemory((address + cpu.m_registers[Register::X]) & 0xFF);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_rotate = cpu.ReadFromMemory(address);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_rotate = cpu.ReadFromMemory(address + cpu.m_registers[Register::X]);
  } else {
    TODO(fmt::format("RotateLeft<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  bool old_carry = cpu.TestStatusFlag(StatusFlag::Carry);
  bool new_carry = value_to_rotate & 0x80;
  uint8_t value_to_store = (value_to_rotate << 1) | (old_carry ? 0x01 : 0x00);

  if constexpr (MODE == AddressingMode::Accumulator) {
    cpu.m_registers[Register::A] = value_to_store;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    cpu.WriteToMemory(address & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    cpu.WriteToMemory((address + cpu.m_registers[Register::X]) & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    cpu.WriteToMemory(address, value_to_store);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    cpu.WriteToMemory(address + cpu.m_registers[Register::X], value_to_store);
  } else {
    TODO(fmt::format("RotateLeft<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.SetStatusFlagValue(StatusFlag::Negative, value_to_store & 0x80);
  cpu.SetStatusFlagValue(StatusFlag::Zero, value_to_store == 0);
  cpu.SetStatusFlagValue(StatusFlag::Carry, new_carry);
}

// ===========================================================================================
// Explicit template instantiations
// ===========================================================================================

// ShiftLeft
template struct CPU::ShiftLeft<AddressingMode::Accumulator>;
template struct CPU::ShiftLeft<AddressingMode::ZeroPage>;
template struct CPU::ShiftLeft<AddressingMode::ZeroPageX>;
template struct CPU::ShiftLeft<AddressingMode::Absolute>;
template struct CPU::ShiftLeft<AddressingMode::AbsoluteX>;

// ShiftRight
template struct CPU::ShiftRight<AddressingMode::Accumulator>;
template struct CPU::ShiftRight<AddressingMode::ZeroPage>;
template struct CPU::ShiftRight<AddressingMode::ZeroPageX>;
template struct CPU::ShiftRight<AddressingMode::Absolute>;
template struct CPU::ShiftRight<AddressingMode::AbsoluteX>;

// RotateRight
template struct CPU::RotateRight<AddressingMode::Accumulator>;
template struct CPU::RotateRight<AddressingMode::ZeroPage>;
template struct CPU::RotateRight<AddressingMode::ZeroPageX>;
template struct CPU::RotateRight<AddressingMode::Absolute>;
template struct CPU::RotateRight<AddressingMode::AbsoluteX>;

// RotateLeft
template struct CPU::RotateLeft<AddressingMode::Accumulator>;
template struct CPU::RotateLeft<AddressingMode::ZeroPage>;
template struct CPU::RotateLeft<AddressingMode::ZeroPageX>;
template struct CPU::RotateLeft<AddressingMode::Absolute>;
template struct CPU::RotateLeft<AddressingMode::AbsoluteX>;
