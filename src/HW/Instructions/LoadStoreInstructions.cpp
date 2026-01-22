#include "HW/CPU.h"
#include <magic_enum.hpp>
#include <spdlog/fmt/fmt.h>

using namespace BNES::HW;

// ===========================================================================================
// LoadRegister - Load a register from memory
// ===========================================================================================

template <CPU::Register REG, AddressingMode MODE> CPU::LoadRegister<REG, MODE>::LoadRegister(uint16_t _value) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::Immediate) {
    this->cycles = 2;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 3;
  } else if constexpr (MODE == AddressingMode::ZeroPageX || MODE == AddressingMode::ZeroPageY) {
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

template <CPU::Register REG, AddressingMode MODE> void CPU::LoadRegister<REG, MODE>::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#LDA (or #LDX,#LDY)

  if constexpr (MODE == AddressingMode::Immediate) {
    cpu.m_registers[REG] = uint8_t(value);
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    // Zero page addressing means the memory address is in the range 0x00 to 0xFF.
    Addr addr = value & 0xFF;
    cpu.m_registers[REG] = cpu.ReadFromMemory(addr);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    // Zero page addressing with X offset means the memory address is in the range 0x00 to 0xFF, and the X register
    // is added to the zero page address.
    // If the result exceeds 0xFF, it wraps around to 0x00.
    Addr addr = (value + cpu.m_registers[Register::X]) & 0xFF;
    cpu.m_registers[REG] = cpu.ReadFromMemory(addr);
  } else if constexpr (MODE == AddressingMode::ZeroPageY) {
    // Zero page addressing with Y offset means the memory address is in the range 0x00 to 0xFF, and the Y register
    // is added to the zero page address.
    // If the result exceeds 0xFF, it wraps around to 0x00.
    Addr addr = (value + cpu.m_registers[Register::Y]) & 0xFF;
    cpu.m_registers[REG] = cpu.ReadFromMemory(addr);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    // Absolute addressing means the memory address is a full 16-bit address (in LE enconding).
    cpu.m_registers[REG] = cpu.ReadFromMemory(value);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    // Indexed absolute addressing means the memory address is a full 16-bit address (in LE enconding) and the X
    // register is added to the zero page address.
    Addr addr = value + cpu.m_registers[Register::X];
    cpu.m_registers[REG] = cpu.ReadFromMemory(addr);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    // Indexed absolute addressing means the memory address is a full 16-bit address (in LE enconding) and the X
    // register is added to the zero page address.
    Addr addr = value + cpu.m_registers[Register::Y];
    cpu.m_registers[REG] = cpu.ReadFromMemory(addr);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    // Indexed indirect addressing is normally used in conjunction with a table of address held on zero page. The
    // address of the table is taken from the instruction and the X register added to it (with zero page wrap around) to
    // give the location of the least significant byte of the target address.

    Addr target_addr_low = (value + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (value + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    cpu.m_registers[REG] = cpu.ReadFromMemory(real_addr);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    // Indirect indexed addressing is the most common indirection mode used on the 6502. In instruction contains the
    // zero page location of the least significant byte of 16 bit address. The Y register is dynamically added to this
    // value to generated the actual target address for operation.

    Addr target_addr_low = value & 0xFF;
    Addr target_addr_high = (value + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    cpu.m_registers[REG] = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]);
  } else {
    TODO(fmt::format("LoadRegister<{},{}>::Apply not implemented", magic_enum::enum_name(REG),
                     magic_enum::enum_name(MODE)));
  }

  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[REG] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[REG] & 0x80);
}

// ===========================================================================================
// LoadAccumulatorAndX - Load both A and X from memory (undocumented)
// ===========================================================================================

template <AddressingMode MODE>
CPU::LoadAccumulatorAndX<MODE>::LoadAccumulatorAndX(uint16_t _value) : DecodedInstruction() {
  this->size = 2;

  if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 3;
  } else if constexpr (MODE == AddressingMode::ZeroPageY) {
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::Absolute || MODE == AddressingMode::AbsoluteY) {
    this->size = 3;
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    this->cycles = 5;
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    this->cycles = 6;
  }

  value = _value;
}

template <AddressingMode MODE> void CPU::LoadAccumulatorAndX<MODE>::Apply(CPU &cpu) const {
  if constexpr (MODE == AddressingMode::ZeroPage) {
    Addr addr = value & 0xFF;
    uint8_t mem_value = cpu.ReadFromMemory(addr);
    cpu.m_registers[Register::A] = cpu.m_registers[Register::X] = mem_value;
  } else if constexpr (MODE == AddressingMode::ZeroPageY) {
    Addr addr = (value + cpu.m_registers[Register::Y]) & 0xFF;
    uint8_t mem_value = cpu.ReadFromMemory(addr);
    cpu.m_registers[Register::A] = cpu.m_registers[Register::X] = mem_value;
  } else if constexpr (MODE == AddressingMode::Absolute) {
    uint8_t mem_value = cpu.ReadFromMemory(value);
    cpu.m_registers[Register::A] = cpu.m_registers[Register::X] = mem_value;
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    Addr addr = value + cpu.m_registers[Register::Y];
    uint8_t mem_value = cpu.ReadFromMemory(addr);
    cpu.m_registers[Register::A] = cpu.m_registers[Register::X] = mem_value;
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (value + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (value + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    uint8_t mem_value = cpu.ReadFromMemory(real_addr);
    cpu.m_registers[Register::A] = cpu.m_registers[Register::X] = mem_value;
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = value & 0xFF;
    Addr target_addr_high = (value + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    uint8_t mem_value = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]);
    cpu.m_registers[Register::A] = cpu.m_registers[Register::X] = mem_value;
  } else {
    TODO(fmt::format("LoadAccumulatorAndX<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::A] & 0x80);
}

// ===========================================================================================
// StoreRegister - Store a register to memory
// ===========================================================================================

template <CPU::Register REG, AddressingMode MODE> CPU::StoreRegister<REG, MODE>::StoreRegister(uint16_t addr) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 3;
  } else if constexpr (MODE == AddressingMode::ZeroPageX || MODE == AddressingMode::ZeroPageY) {
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::Absolute) {
    this->size = 3;
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::AbsoluteX || MODE == AddressingMode::AbsoluteY) {
    this->size = 3;
    this->cycles = 5;
  } else if constexpr (MODE == AddressingMode::IndirectX || MODE == AddressingMode::IndirectY) {
    this->cycles = 6;
  }

  address = addr;
}

template <CPU::Register REG, AddressingMode MODE> void CPU::StoreRegister<REG, MODE>::Apply(CPU &cpu) const {
  if constexpr (MODE == AddressingMode::ZeroPage) {
    Addr addr = address & 0xFF;
    cpu.WriteToMemory(addr, cpu.m_registers[REG]);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    Addr addr = (address + cpu.m_registers[Register::X]) & 0xFF;
    cpu.WriteToMemory(addr, cpu.m_registers[REG]);
  } else if constexpr (MODE == AddressingMode::ZeroPageY) {
    Addr addr = (address + cpu.m_registers[Register::Y]) & 0xFF;
    cpu.WriteToMemory(addr, cpu.m_registers[REG]);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    cpu.WriteToMemory(address, cpu.m_registers[REG]);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    Addr addr = address + cpu.m_registers[Register::X];
    cpu.WriteToMemory(addr, cpu.m_registers[REG]);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    Addr addr = address + cpu.m_registers[Register::Y];
    cpu.WriteToMemory(addr, cpu.m_registers[REG]);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (address + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (address + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    cpu.WriteToMemory(real_addr, cpu.m_registers[REG]);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    Addr target_addr_low = address & 0xFF;
    Addr target_addr_high = (address + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    cpu.WriteToMemory(real_addr + cpu.m_registers[Register::Y], cpu.m_registers[REG]);
  } else {
    TODO(fmt::format("StoreRegister<{},{}>::Apply not implemented", magic_enum::enum_name(REG),
                     magic_enum::enum_name(MODE)));
  }
}

// ===========================================================================================
// StoreAccumulatorAndX - Store A AND X to memory (undocumented)
// ===========================================================================================

template <AddressingMode MODE>
CPU::StoreAccumulatorAndX<MODE>::StoreAccumulatorAndX(uint16_t addr) : DecodedInstruction() {
  this->size = 2;

  if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 3;
  } else if constexpr (MODE == AddressingMode::ZeroPageY) {
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::Absolute) {
    this->size = 3;
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    this->cycles = 6;
  }

  address = addr;
}

template <AddressingMode MODE> void CPU::StoreAccumulatorAndX<MODE>::Apply(CPU &cpu) const {
  if constexpr (MODE == AddressingMode::ZeroPage) {
    Addr addr = address & 0xFF;
    cpu.WriteToMemory(addr, cpu.m_registers[Register::X] & cpu.m_registers[Register::A]);
  } else if constexpr (MODE == AddressingMode::ZeroPageY) {
    Addr addr = (address + cpu.m_registers[Register::Y]) & 0xFF;
    cpu.WriteToMemory(addr, cpu.m_registers[Register::X] & cpu.m_registers[Register::A]);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    cpu.WriteToMemory(address, cpu.m_registers[Register::X] & cpu.m_registers[Register::A]);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    Addr target_addr_low = (address + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (address + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    cpu.WriteToMemory(real_addr, cpu.m_registers[Register::X] & cpu.m_registers[Register::A]);
  } else {
    TODO(fmt::format("StoreAccumulatorAndX<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }
}

// ===========================================================================================
// Explicit template instantiations
// ===========================================================================================

// LoadRegister
template struct CPU::LoadRegister<CPU::Register::A, AddressingMode::Immediate>;
template struct CPU::LoadRegister<CPU::Register::A, AddressingMode::ZeroPage>;
template struct CPU::LoadRegister<CPU::Register::A, AddressingMode::ZeroPageX>;
template struct CPU::LoadRegister<CPU::Register::A, AddressingMode::Absolute>;
template struct CPU::LoadRegister<CPU::Register::A, AddressingMode::AbsoluteX>;
template struct CPU::LoadRegister<CPU::Register::A, AddressingMode::AbsoluteY>;
template struct CPU::LoadRegister<CPU::Register::A, AddressingMode::IndirectX>;
template struct CPU::LoadRegister<CPU::Register::A, AddressingMode::IndirectY>;
template struct CPU::LoadRegister<CPU::Register::X, AddressingMode::Immediate>;
template struct CPU::LoadRegister<CPU::Register::X, AddressingMode::ZeroPage>;
template struct CPU::LoadRegister<CPU::Register::X, AddressingMode::ZeroPageY>;
template struct CPU::LoadRegister<CPU::Register::X, AddressingMode::Absolute>;
template struct CPU::LoadRegister<CPU::Register::X, AddressingMode::AbsoluteY>;
template struct CPU::LoadRegister<CPU::Register::Y, AddressingMode::Immediate>;
template struct CPU::LoadRegister<CPU::Register::Y, AddressingMode::ZeroPage>;
template struct CPU::LoadRegister<CPU::Register::Y, AddressingMode::ZeroPageX>;
template struct CPU::LoadRegister<CPU::Register::Y, AddressingMode::Absolute>;
template struct CPU::LoadRegister<CPU::Register::Y, AddressingMode::AbsoluteX>;

// LoadAccumulatorAndX (undocumented)
template struct CPU::LoadAccumulatorAndX<AddressingMode::ZeroPage>;
template struct CPU::LoadAccumulatorAndX<AddressingMode::ZeroPageY>;
template struct CPU::LoadAccumulatorAndX<AddressingMode::Absolute>;
template struct CPU::LoadAccumulatorAndX<AddressingMode::AbsoluteY>;
template struct CPU::LoadAccumulatorAndX<AddressingMode::IndirectX>;
template struct CPU::LoadAccumulatorAndX<AddressingMode::IndirectY>;

// StoreRegister
template struct CPU::StoreRegister<CPU::Register::A, AddressingMode::ZeroPage>;
template struct CPU::StoreRegister<CPU::Register::X, AddressingMode::ZeroPage>;
template struct CPU::StoreRegister<CPU::Register::Y, AddressingMode::ZeroPage>;
template struct CPU::StoreRegister<CPU::Register::A, AddressingMode::ZeroPageX>;
template struct CPU::StoreRegister<CPU::Register::X, AddressingMode::ZeroPageY>;
template struct CPU::StoreRegister<CPU::Register::Y, AddressingMode::ZeroPageX>;
template struct CPU::StoreRegister<CPU::Register::A, AddressingMode::Absolute>;
template struct CPU::StoreRegister<CPU::Register::X, AddressingMode::Absolute>;
template struct CPU::StoreRegister<CPU::Register::Y, AddressingMode::Absolute>;
template struct CPU::StoreRegister<CPU::Register::A, AddressingMode::AbsoluteX>;
template struct CPU::StoreRegister<CPU::Register::A, AddressingMode::AbsoluteY>;
template struct CPU::StoreRegister<CPU::Register::A, AddressingMode::IndirectX>;
template struct CPU::StoreRegister<CPU::Register::A, AddressingMode::IndirectY>;

// StoreAccumulatorAndX (undocumented)
template struct CPU::StoreAccumulatorAndX<AddressingMode::ZeroPage>;
template struct CPU::StoreAccumulatorAndX<AddressingMode::ZeroPageY>;
template struct CPU::StoreAccumulatorAndX<AddressingMode::Absolute>;
template struct CPU::StoreAccumulatorAndX<AddressingMode::IndirectX>;
