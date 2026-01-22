#include "HW/CPU.h"
#include "common/Utils.h"
#include <magic_enum.hpp>
#include <spdlog/fmt/fmt.h>

using namespace BNES::HW;

// ===========================================================================================
// Jump - Jump to address
// ===========================================================================================

template <AddressingMode MODE> CPU::Jump<MODE>::Jump(uint16_t addr) {
  this->size = 3;

  if constexpr (MODE == AddressingMode::Absolute) {
    this->cycles = 3;
  } else if constexpr (MODE == AddressingMode::Indirect) {
    this->cycles = 5;
  } else {
    std::unreachable();
  }

  address = addr;
}

template <AddressingMode MODE> void CPU::Jump<MODE>::Apply(CPU &cpu) const {
  if constexpr (MODE == AddressingMode::Absolute) {
    cpu.m_program_counter = address;
  } else if constexpr (MODE == AddressingMode::Indirect) {
    Addr low_byte = cpu.ReadFromMemory(address);
    Addr high_byte = cpu.ReadFromMemory((address & 0xFF00) | ((address + 1) & 0x00FF));
    cpu.m_program_counter = (high_byte << 8) | low_byte;
  } else {
    TODO(fmt::format("Jump<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }
}

// ===========================================================================================
// Branch - Conditional branch
// ===========================================================================================

template <Conditional COND> void CPU::Branch<COND>::Apply(CPU &cpu) {
  bool should_branch = false;
  if constexpr (COND == Conditional::Equal) {
    should_branch = cpu.TestStatusFlag(StatusFlag::Zero);
  } else if constexpr (COND == Conditional::NotEqual) {
    should_branch = !cpu.TestStatusFlag(StatusFlag::Zero);
  } else if constexpr (COND == Conditional::CarrySet) {
    should_branch = cpu.TestStatusFlag(StatusFlag::Carry);
  } else if constexpr (COND == Conditional::CarryClear) {
    should_branch = !cpu.TestStatusFlag(StatusFlag::Carry);
  } else if constexpr (COND == Conditional::Minus) {
    should_branch = cpu.TestStatusFlag(StatusFlag::Negative);
  } else if constexpr (COND == Conditional::Positive) {
    should_branch = !cpu.TestStatusFlag(StatusFlag::Negative);
  } else if constexpr (COND == Conditional::OverflowSet) {
    should_branch = cpu.TestStatusFlag(StatusFlag::Overflow);
  } else if constexpr (COND == Conditional::OverflowClear) {
    should_branch = !cpu.TestStatusFlag(StatusFlag::Overflow);
  } else {
    TODO(fmt::format("Branch<{}>::Apply not implemented", magic_enum::enum_name(COND)));
  }

  if (!should_branch) {
    return;
  }

  // Check if we cross a page boundary (different high byte)
  uint16_t old_pc = cpu.m_program_counter;
  uint16_t new_pc = cpu.m_program_counter + int16_t(offset);

  // If the high byte changes, we crossed a page boundary and need an extra cycle
  if ((old_pc & 0xFF00) != (new_pc & 0xFF00)) {
    // Page boundary crossed, but we can't modify cycles here since this is const
    // The CPU main loop would need to handle this case
    this->cycles += 1;
  }

  this->cycles += 1;

  cpu.m_program_counter += int16_t(offset);
}

// ===========================================================================================
// Subroutine and interrupt handling
// ===========================================================================================

void CPU::Break::Apply([[maybe_unused]] CPU &cpu) const {
  // FIXME: Not elegant, but to break from the main CPU loop we throw an exception.
  //        Since we are throwing, the program_counter will not be updated by the main loop.
  //        Let's do it here.
  cpu.m_program_counter += size;
  throw NonMaskableInterrupt{};
}

void CPU::JumpToSubroutine::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#JSR

  Addr target_address{0};
  target_address = address;

  // Push the return address (address of the next instruction - 1) onto the stack
  uint16_t return_address = cpu.m_program_counter + size - 1;
  cpu.WriteToMemory(StackBaseAddress + cpu.m_stack_pointer, (return_address >> 8) & 0xFF); // Push high byte
  cpu.m_stack_pointer--;
  cpu.WriteToMemory(StackBaseAddress + cpu.m_stack_pointer, return_address & 0xFF); // Push low byte
  cpu.m_stack_pointer--;

  // Jump to the target address
  cpu.m_program_counter = target_address;
}

void CPU::ReturnFromSubroutine::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#RTS

  // Pull the return address from the stack
  cpu.m_stack_pointer++;
  uint8_t low_byte = cpu.ReadFromMemory(StackBaseAddress + cpu.m_stack_pointer);
  cpu.m_stack_pointer++;
  uint8_t high_byte = cpu.ReadFromMemory(StackBaseAddress + cpu.m_stack_pointer);

  uint16_t return_address = (high_byte << 8) | low_byte;

  // Set the program counter to the return address + 1
  cpu.m_program_counter = return_address + 1;
}

void CPU::ReturnFromInterrupt::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#RTI

  // Pull the status word from the stack
  cpu.m_stack_pointer++;
  cpu.m_status = (cpu.ReadFromMemory(StackBaseAddress + cpu.m_stack_pointer) & 0xEF) | 0x20;
  // Pull the program counter from the stack
  cpu.m_stack_pointer++;
  uint8_t low_byte = cpu.ReadFromMemory(StackBaseAddress + cpu.m_stack_pointer);
  cpu.m_stack_pointer++;
  uint8_t high_byte = cpu.ReadFromMemory(StackBaseAddress + cpu.m_stack_pointer);

  uint16_t return_address = (high_byte << 8) | low_byte;

  // Set the program counter to the return address
  cpu.m_program_counter = return_address;
}

// ===========================================================================================
// Stack operations
// ===========================================================================================

void CPU::PushAccumulator::Apply(CPU &cpu) const {
  cpu.WriteToMemory(StackBaseAddress + cpu.m_stack_pointer, cpu.m_registers[Register::A]);
  cpu.m_stack_pointer--;
}

void CPU::PullAccumulator::Apply(CPU &cpu) const {
  cpu.m_stack_pointer++;
  cpu.m_registers[Register::A] = cpu.ReadFromMemory(StackBaseAddress + cpu.m_stack_pointer);

  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, (cpu.m_registers[Register::A] & 0x80) != 0);
}

void CPU::PushStatusRegister::Apply(CPU &cpu) const {
  uint8_t status_word = cpu.m_status.to_ulong() | 0x30;
  cpu.WriteToMemory(StackBaseAddress + cpu.m_stack_pointer, status_word);
  cpu.m_stack_pointer--;
}

void CPU::PullStatusRegister::Apply(CPU &cpu) const {
  cpu.m_stack_pointer++;
  cpu.m_status = (cpu.ReadFromMemory(StackBaseAddress + cpu.m_stack_pointer) & 0xEF) | 0x20;

  // TODO: Note that the effect of changing I is delayed one instruction because the flag is changed after IRQ is
  //       polled, delaying the effect until IRQ is polled in the next instruction like with CLI and SEI.
}

// ===========================================================================================
// Explicit template instantiations
// ===========================================================================================

// Jump
template struct CPU::Jump<AddressingMode::Absolute>;
template struct CPU::Jump<AddressingMode::Indirect>;

// Branch
template struct CPU::Branch<Conditional::Equal>;
template struct CPU::Branch<Conditional::NotEqual>;
template struct CPU::Branch<Conditional::CarrySet>;
template struct CPU::Branch<Conditional::CarryClear>;
template struct CPU::Branch<Conditional::Minus>;
template struct CPU::Branch<Conditional::Positive>;
template struct CPU::Branch<Conditional::OverflowSet>;
template struct CPU::Branch<Conditional::OverflowClear>;

