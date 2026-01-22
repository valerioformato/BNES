#include "HW/CPU.h"
#include "common/Utils.h"
#include <magic_enum.hpp>

using namespace BNES::HW;

// ===== DecrementAndCompare =====

template <AddressingMode MODE> CPU::DecrementAndCompare<MODE>::DecrementAndCompare(uint16_t addr) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 5;
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::Absolute) {
    this->size = 3;
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::AbsoluteX || MODE == AddressingMode::AbsoluteY) {
    this->size = 3;
    this->cycles = 7;
  } else if constexpr (MODE == AddressingMode::IndirectX || MODE == AddressingMode::IndirectY) {
    this->cycles = 8;
  } else {
    std::unreachable();
  }

  address = addr;
}

template <AddressingMode MODE> void CPU::DecrementAndCompare<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_compare = 0;
  if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_compare = cpu.ReadFromMemory(address & 0xFF) - 1;
    cpu.WriteToMemory(address & 0xFF, value_to_compare);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_compare = cpu.ReadFromMemory((address + cpu.m_registers[Register::X]) & 0xFF) - 1;
    cpu.WriteToMemory((address + cpu.m_registers[Register::X]) & 0xFF, value_to_compare);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_compare = cpu.ReadFromMemory(address) - 1;
    cpu.WriteToMemory(address, value_to_compare);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_compare = cpu.ReadFromMemory(address + cpu.m_registers[Register::X]) - 1;
    cpu.WriteToMemory(address + cpu.m_registers[Register::X], value_to_compare);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    value_to_compare = cpu.ReadFromMemory(address + cpu.m_registers[Register::Y]) - 1;
    cpu.WriteToMemory(address + cpu.m_registers[Register::Y], value_to_compare);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (address + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (address + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_compare = cpu.ReadFromMemory(real_addr) - 1;
    cpu.WriteToMemory(real_addr, value_to_compare);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = address & 0xFF;
    Addr target_addr_high = (address + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_compare = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]) - 1;
    cpu.WriteToMemory(real_addr + cpu.m_registers[Register::Y], value_to_compare);
  } else {
    TODO(fmt::format("DecrementAndCompare<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.SetStatusFlagValue(StatusFlag::Carry, cpu.m_registers[Register::A] >= value_to_compare);
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == value_to_compare);
  cpu.SetStatusFlagValue(StatusFlag::Negative, (cpu.m_registers[Register::A] - value_to_compare) & 0x80);
}

// ===== IncrementAndSubtract =====

template <AddressingMode MODE> CPU::IncrementAndSubtract<MODE>::IncrementAndSubtract(uint16_t addr) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 5;
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::Absolute) {
    this->size = 3;
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::AbsoluteX || MODE == AddressingMode::AbsoluteY) {
    this->size = 3;
    this->cycles = 7;
  } else if constexpr (MODE == AddressingMode::IndirectX || MODE == AddressingMode::IndirectY) {
    this->cycles = 8;
  } else {
    std::unreachable();
  }

  address = addr;
}

template <AddressingMode MODE> void CPU::IncrementAndSubtract<MODE>::Apply(CPU &cpu) const {
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
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    value_to_write = cpu.ReadFromMemory(address + cpu.m_registers[Register::Y]) + 1;
    cpu.WriteToMemory(address + cpu.m_registers[Register::Y], value_to_write);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (address + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (address + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_write = cpu.ReadFromMemory(real_addr) + 1;
    cpu.WriteToMemory(real_addr, value_to_write);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = address & 0xFF;
    Addr target_addr_high = (address + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_write = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]) + 1;
    cpu.WriteToMemory(real_addr + cpu.m_registers[Register::Y], value_to_write);
  } else {
    TODO(fmt::format("IncrementAndSubtract<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  uint16_t intermediate_result =
      cpu.m_registers[Register::A] - value_to_write - (1 - cpu.TestStatusFlag(StatusFlag::Carry));

  // These will be useful to compute the overflow bit
  // NOTE: This is actually quite tricky, see https://www.righto.com/2012/12/the-6502-overflow-flag-explained.html for a
  // full explanation of the overflow bit
  // For SBC, we convert to addition: A - B = A + (~B) + 1 (with carry acting as the +1)
  uint8_t M = cpu.m_registers[Register::A];             // M is the original value in the accumulator
  uint8_t N = ~(value_to_write & 0xFF);                 // N is the one's complement of the value being subtracted
  uint8_t result = uint8_t(intermediate_result & 0xFF); // Result is the final value after subtraction

  cpu.m_registers[Register::A] = result;

  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::A] & 0x80);
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Carry, !(intermediate_result & 0x100)); // Inverted for subtraction
  // Overflow formula is the same as ADC when using one's complement
  cpu.SetStatusFlagValue(StatusFlag::Overflow, ((M ^ result) & (N ^ result) & 0x80) != 0);
}

// ===== ShiftLeftAndOR =====

template <AddressingMode MODE> CPU::ShiftLeftAndOR<MODE>::ShiftLeftAndOR(uint16_t addr) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 5;
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::Absolute) {
    this->size = 3;
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::AbsoluteX || MODE == AddressingMode::AbsoluteY) {
    this->size = 3;
    this->cycles = 7;
  } else if constexpr (MODE == AddressingMode::IndirectX || MODE == AddressingMode::IndirectY) {
    this->cycles = 8;
  } else {
    std::unreachable();
  }

  address = addr;
}

template <AddressingMode MODE> void CPU::ShiftLeftAndOR<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_shift = 0;
  if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_shift = cpu.ReadFromMemory(address & 0xFF);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_shift = cpu.ReadFromMemory((address + cpu.m_registers[Register::X]) & 0xFF);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_shift = cpu.ReadFromMemory(address);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_shift = cpu.ReadFromMemory(address + cpu.m_registers[Register::X]);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    value_to_shift = cpu.ReadFromMemory(address + cpu.m_registers[Register::Y]);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (address + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (address + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_shift = cpu.ReadFromMemory(real_addr);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = address & 0xFF;
    Addr target_addr_high = (address + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_shift = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]);
  } else {
    TODO(fmt::format("ShiftLeftAndOR<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  uint16_t shifted_value = value_to_shift << 1;
  uint8_t value_to_store = shifted_value & 0xFF;

  if constexpr (MODE == AddressingMode::ZeroPage) {
    cpu.WriteToMemory(address & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    cpu.WriteToMemory((address + cpu.m_registers[Register::X]) & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    cpu.WriteToMemory(address, value_to_store);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    cpu.WriteToMemory(address + cpu.m_registers[Register::X], value_to_store);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    cpu.WriteToMemory(address + cpu.m_registers[Register::Y], value_to_store);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (address + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (address + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    cpu.WriteToMemory(real_addr, value_to_store);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = address & 0xFF;
    Addr target_addr_high = (address + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    cpu.WriteToMemory(real_addr + cpu.m_registers[Register::Y], value_to_store);
  } else {
    TODO(fmt::format("ShiftLeftAndOR<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.m_registers[Register::A] |= value_to_store;

  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::A] & 0x80);
  cpu.SetStatusFlagValue(StatusFlag::Carry, shifted_value & 0x100);
}

// ===== RotateLeftAndAND =====

template <AddressingMode MODE> CPU::RotateLeftAndAND<MODE>::RotateLeftAndAND(uint16_t addr) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 5;
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::Absolute) {
    this->size = 3;
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::AbsoluteX || MODE == AddressingMode::AbsoluteY) {
    this->size = 3;
    this->cycles = 7;
  } else if constexpr (MODE == AddressingMode::IndirectX || MODE == AddressingMode::IndirectY) {
    this->cycles = 8;
  } else {
    std::unreachable();
  }

  address = addr;
}

template <AddressingMode MODE> void CPU::RotateLeftAndAND<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_rotate = 0;
  if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_rotate = cpu.ReadFromMemory(address & 0xFF);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_rotate = cpu.ReadFromMemory((address + cpu.m_registers[Register::X]) & 0xFF);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_rotate = cpu.ReadFromMemory(address);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_rotate = cpu.ReadFromMemory(address + cpu.m_registers[Register::X]);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    value_to_rotate = cpu.ReadFromMemory(address + cpu.m_registers[Register::Y]);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (address + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (address + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_rotate = cpu.ReadFromMemory(real_addr);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = address & 0xFF;
    Addr target_addr_high = (address + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_rotate = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]);
  } else {
    TODO(fmt::format("RotateLeftAndAND<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  bool old_carry = cpu.TestStatusFlag(StatusFlag::Carry);
  bool new_carry = value_to_rotate & 0x80;
  uint8_t value_to_store = (value_to_rotate << 1) | (old_carry ? 0x01 : 0x00);

  if constexpr (MODE == AddressingMode::ZeroPage) {
    cpu.WriteToMemory(address & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    cpu.WriteToMemory((address + cpu.m_registers[Register::X]) & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    cpu.WriteToMemory(address, value_to_store);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    cpu.WriteToMemory(address + cpu.m_registers[Register::X], value_to_store);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    cpu.WriteToMemory(address + cpu.m_registers[Register::Y], value_to_store);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (address + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (address + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    cpu.WriteToMemory(real_addr, value_to_store);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = address & 0xFF;
    Addr target_addr_high = (address + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    cpu.WriteToMemory(real_addr + cpu.m_registers[Register::Y], value_to_store);
  } else {
    TODO(fmt::format("RotateLeftAndAND<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.m_registers[Register::A] &= value_to_store;

  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::A] & 0x80);
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Carry, new_carry);
}

// ===== ShiftRightAndEOR =====

template <AddressingMode MODE> CPU::ShiftRightAndEOR<MODE>::ShiftRightAndEOR(uint16_t addr) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 5;
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::Absolute) {
    this->size = 3;
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::AbsoluteX || MODE == AddressingMode::AbsoluteY) {
    this->size = 3;
    this->cycles = 7;
  } else if constexpr (MODE == AddressingMode::IndirectX || MODE == AddressingMode::IndirectY) {
    this->cycles = 8;
  } else {
    std::unreachable();
  }

  address = addr;
}

template <AddressingMode MODE> void CPU::ShiftRightAndEOR<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_shift = 0;
  if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_shift = cpu.ReadFromMemory(address & 0xFF);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_shift = cpu.ReadFromMemory((address + cpu.m_registers[Register::X]) & 0xFF);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_shift = cpu.ReadFromMemory(address);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_shift = cpu.ReadFromMemory(address + cpu.m_registers[Register::X]);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    value_to_shift = cpu.ReadFromMemory(address + cpu.m_registers[Register::Y]);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (address + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (address + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_shift = cpu.ReadFromMemory(real_addr);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = address & 0xFF;
    Addr target_addr_high = (address + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_shift = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]);
  } else {
    TODO(fmt::format("ShiftRightAndEOR<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  uint8_t value_to_store = (value_to_shift >> 1) & 0xFF;

  cpu.SetStatusFlagValue(StatusFlag::Carry, value_to_shift & 0x1);

  if constexpr (MODE == AddressingMode::ZeroPage) {
    cpu.WriteToMemory(address & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    cpu.WriteToMemory((address + cpu.m_registers[Register::X]) & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    cpu.WriteToMemory(address, value_to_store);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    cpu.WriteToMemory(address + cpu.m_registers[Register::X], value_to_store);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    cpu.WriteToMemory(address + cpu.m_registers[Register::Y], value_to_store);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (address + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (address + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    cpu.WriteToMemory(real_addr, value_to_store);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = address & 0xFF;
    Addr target_addr_high = (address + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    cpu.WriteToMemory(real_addr + cpu.m_registers[Register::Y], value_to_store);
  } else {
    TODO(fmt::format("ShiftRightAndEOR<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.m_registers[Register::A] ^= value_to_store;

  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::A] & 0x80);
}

// ===== RotateRightAndAdd =====

template <AddressingMode MODE> CPU::RotateRightAndAdd<MODE>::RotateRightAndAdd(uint16_t addr) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 5;
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::Absolute) {
    this->size = 3;
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::AbsoluteX || MODE == AddressingMode::AbsoluteY) {
    this->size = 3;
    this->cycles = 7;
  } else if constexpr (MODE == AddressingMode::IndirectX || MODE == AddressingMode::IndirectY) {
    this->cycles = 8;
  } else {
    std::unreachable();
  }

  address = addr;
}

template <AddressingMode MODE> void CPU::RotateRightAndAdd<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_rotate = 0;
  if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_rotate = cpu.ReadFromMemory(address & 0xFF);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    value_to_rotate = cpu.ReadFromMemory((address + cpu.m_registers[Register::X]) & 0xFF);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_rotate = cpu.ReadFromMemory(address);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    value_to_rotate = cpu.ReadFromMemory(address + cpu.m_registers[Register::X]);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    value_to_rotate = cpu.ReadFromMemory(address + cpu.m_registers[Register::Y]);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (address + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (address + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_rotate = cpu.ReadFromMemory(real_addr);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = address & 0xFF;
    Addr target_addr_high = (address + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_rotate = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]);
  } else {
    TODO(fmt::format("RotateRightAndAdd<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  bool old_carry = cpu.TestStatusFlag(StatusFlag::Carry);
  bool new_carry = value_to_rotate & 0x1;
  uint8_t value_to_store = (value_to_rotate >> 1) | (old_carry ? 0x80 : 0x00);

  if constexpr (MODE == AddressingMode::ZeroPage) {
    cpu.WriteToMemory(address & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    cpu.WriteToMemory((address + cpu.m_registers[Register::X]) & 0xFF, value_to_store);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    cpu.WriteToMemory(address, value_to_store);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    cpu.WriteToMemory(address + cpu.m_registers[Register::X], value_to_store);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    cpu.WriteToMemory(address + cpu.m_registers[Register::Y], value_to_store);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (address + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (address + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    cpu.WriteToMemory(real_addr, value_to_store);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = address & 0xFF;
    Addr target_addr_high = (address + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    cpu.WriteToMemory(real_addr + cpu.m_registers[Register::Y], value_to_store);
  } else {
    TODO(fmt::format("RotateRightAndAdd<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.SetStatusFlagValue(StatusFlag::Carry, new_carry);

  uint16_t intermediate_result = cpu.m_registers[Register::A] + value_to_store + cpu.TestStatusFlag(StatusFlag::Carry);

  // These will be useful to compute the overflow bit
  // NOTE: This is actually quite tricky, see https://www.righto.com/2012/12/the-6502-overflow-flag-explained.html for a
  // full explanation of the overflow bit
  uint8_t M = cpu.m_registers[Register::A];             // M is the original value in the accumulator
  uint8_t N = value_to_store & 0xFF;                    // N is the value being added
  uint8_t result = uint8_t(intermediate_result & 0xFF); // Result is the final value after addition

  cpu.m_registers[Register::A] = result;

  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::A] & 0x80);
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Carry, intermediate_result & 0x100);
  // Overflow if both operands are positive and result is negative, or both operands are negative and result is
  // positive.
  cpu.SetStatusFlagValue(StatusFlag::Overflow, ((M ^ result) & (N ^ result) & 0x80) != 0);
}

// ===== Explicit template instantiations =====

template struct CPU::DecrementAndCompare<AddressingMode::ZeroPage>;
template struct CPU::DecrementAndCompare<AddressingMode::ZeroPageX>;
template struct CPU::DecrementAndCompare<AddressingMode::Absolute>;
template struct CPU::DecrementAndCompare<AddressingMode::AbsoluteX>;
template struct CPU::DecrementAndCompare<AddressingMode::AbsoluteY>;
template struct CPU::DecrementAndCompare<AddressingMode::IndirectX>;
template struct CPU::DecrementAndCompare<AddressingMode::IndirectY>;

template struct CPU::IncrementAndSubtract<AddressingMode::ZeroPage>;
template struct CPU::IncrementAndSubtract<AddressingMode::ZeroPageX>;
template struct CPU::IncrementAndSubtract<AddressingMode::Absolute>;
template struct CPU::IncrementAndSubtract<AddressingMode::AbsoluteX>;
template struct CPU::IncrementAndSubtract<AddressingMode::AbsoluteY>;
template struct CPU::IncrementAndSubtract<AddressingMode::IndirectX>;
template struct CPU::IncrementAndSubtract<AddressingMode::IndirectY>;

template struct CPU::ShiftLeftAndOR<AddressingMode::ZeroPage>;
template struct CPU::ShiftLeftAndOR<AddressingMode::ZeroPageX>;
template struct CPU::ShiftLeftAndOR<AddressingMode::Absolute>;
template struct CPU::ShiftLeftAndOR<AddressingMode::AbsoluteX>;
template struct CPU::ShiftLeftAndOR<AddressingMode::AbsoluteY>;
template struct CPU::ShiftLeftAndOR<AddressingMode::IndirectX>;
template struct CPU::ShiftLeftAndOR<AddressingMode::IndirectY>;

template struct CPU::RotateLeftAndAND<AddressingMode::ZeroPage>;
template struct CPU::RotateLeftAndAND<AddressingMode::ZeroPageX>;
template struct CPU::RotateLeftAndAND<AddressingMode::Absolute>;
template struct CPU::RotateLeftAndAND<AddressingMode::AbsoluteX>;
template struct CPU::RotateLeftAndAND<AddressingMode::AbsoluteY>;
template struct CPU::RotateLeftAndAND<AddressingMode::IndirectX>;
template struct CPU::RotateLeftAndAND<AddressingMode::IndirectY>;

template struct CPU::ShiftRightAndEOR<AddressingMode::ZeroPage>;
template struct CPU::ShiftRightAndEOR<AddressingMode::ZeroPageX>;
template struct CPU::ShiftRightAndEOR<AddressingMode::Absolute>;
template struct CPU::ShiftRightAndEOR<AddressingMode::AbsoluteX>;
template struct CPU::ShiftRightAndEOR<AddressingMode::AbsoluteY>;
template struct CPU::ShiftRightAndEOR<AddressingMode::IndirectX>;
template struct CPU::ShiftRightAndEOR<AddressingMode::IndirectY>;

template struct CPU::RotateRightAndAdd<AddressingMode::ZeroPage>;
template struct CPU::RotateRightAndAdd<AddressingMode::ZeroPageX>;
template struct CPU::RotateRightAndAdd<AddressingMode::Absolute>;
template struct CPU::RotateRightAndAdd<AddressingMode::AbsoluteX>;
template struct CPU::RotateRightAndAdd<AddressingMode::AbsoluteY>;
template struct CPU::RotateRightAndAdd<AddressingMode::IndirectX>;
template struct CPU::RotateRightAndAdd<AddressingMode::IndirectY>;
