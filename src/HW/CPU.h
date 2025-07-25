#ifndef BNES_HW_CPU_H
#define BNES_HW_CPU_H

#include "HW/OpCodes.h"
#include "common/EnumArray.h"
#include "common/Utils.h"

#include <bitset>
#include <cstdint>
#include <span>
#include <variant>

namespace BNES::HW {
class CPU {
public:
  using Addr = uint16_t;

  enum class Register : uint8_t { A = 0, X, Y };
  enum class StatusFlag : uint8_t {
    Carry = 0,
    Zero,
    InterruptDisable,
    DecimalMode,
    BreakCommand,
    Overflow = 6,
    Negative
  };

  static constexpr size_t STACK_MEM_SIZE = 2048;
  static constexpr size_t PROG_MEM_SIZE = 32767;

  ErrorOr<void> LoadProgram(std::vector<uint8_t> program);

  // Helper functions to inspect the state of the CPU
  [[nodiscard]] EnumArray<uint8_t, Register> Registers() const { return m_registers; };
  [[nodiscard]] uint8_t StackPointer() const { return m_stack_pointer; }
  [[nodiscard]] Addr ProgramCounter() const { return m_program_counter; }
  [[nodiscard]] std::bitset<8> StatusFlags() const { return m_status; }
  [[nodiscard]] bool TestStatusFlag(StatusFlag flag) const { return m_status.test(static_cast<size_t>(flag)); }

private:
  static constexpr Addr StackBaseAddress{0x0100}; // Base address for the stack
  static constexpr Addr ProgramBaseAddress{0x8000};

  EnumArray<uint8_t, Register> m_registers{}; // Array to hold CPU registers A, X, and Y
  std::bitset<8> m_status{0x00};              // Status register (flags)
  uint8_t m_stack_pointer{0xFF};              // Stack pointer initialized to 0xFF
  Addr m_program_counter{ProgramBaseAddress}; // Program counter

  std::array<uint8_t, STACK_MEM_SIZE> m_ram_memory{}; // CPU memory (2kB)
  std::array<uint8_t, PROG_MEM_SIZE> m_program_memory{};

protected:
  class NonMaskableInterrupt : public std::exception {};

  // Mostly used for unit testing purposes
  [[nodiscard]] uint8_t ReadFromMemory(Addr addr) const;
  void WriteToMemory(Addr addr, uint8_t value);
  void SetRegister(Register reg, uint8_t value);
  void SetStatusFlag(StatusFlag flag, bool value) { m_status.set(static_cast<size_t>(flag), value); }
  void ToggleStatusFlag(StatusFlag flag) { m_status.flip(static_cast<size_t>(flag)); }

  [[nodiscard]] const std::array<uint8_t, PROG_MEM_SIZE> &ProgramMemory() const { return m_program_memory; }

public:
  template <typename Type> struct DecodedInstruction {
    uint8_t size;
    uint8_t cycles;
  };

  struct Break : DecodedInstruction<Break> {
    Break() : DecodedInstruction<Break>(1, 7) {}
    inline void Apply([[maybe_unused]] CPU &cpu) const;
  };

  template <Register REG, AddressingMode MODE> struct LoadRegister : DecodedInstruction<LoadRegister<REG, MODE>> {
    LoadRegister() = delete;
    explicit LoadRegister(uint16_t);

    void Apply(CPU &cpu) const;

    uint16_t value{0};
  };

  template <Register REG, AddressingMode MODE> struct StoreRegister : DecodedInstruction<StoreRegister<REG, MODE>> {
    StoreRegister() = delete;
    explicit StoreRegister(uint16_t addr);

    void Apply(CPU &cpu) const;

    uint16_t address{0};
  };

  template <AddressingMode MODE> struct AddWithCarry : DecodedInstruction<AddWithCarry<MODE>> {
    AddWithCarry() = delete;
    explicit AddWithCarry(uint16_t);

    void Apply(CPU &cpu) const;

    uint16_t value{0};
  };

  template <Register REG> struct IncrementRegister : DecodedInstruction<IncrementRegister<REG>> {
    IncrementRegister() : DecodedInstruction<IncrementRegister>(1, 2) {}
    void Apply(CPU &cpu) const;
  };

  template <Register REG> struct TransferAccumulatorTo : DecodedInstruction<TransferAccumulatorTo<REG>> {
    TransferAccumulatorTo() : DecodedInstruction<TransferAccumulatorTo>(1, 2) {}
    void Apply(CPU &cpu) const;
  };

  // clang-format off
  using Instruction = std::variant<
      Break,
      // Loads
      LoadRegister<Register::A, AddressingMode::Immediate>,
      LoadRegister<Register::A, AddressingMode::Absolute>,
      LoadRegister<Register::A, AddressingMode::AbsoluteX>,
      LoadRegister<Register::A, AddressingMode::AbsoluteY>,
      LoadRegister<Register::A, AddressingMode::ZeroPage>,
      LoadRegister<Register::A, AddressingMode::ZeroPageX>,
      LoadRegister<Register::A, AddressingMode::IndirectX>,
      LoadRegister<Register::A, AddressingMode::IndirectY>,
      LoadRegister<Register::X, AddressingMode::Immediate>,
      LoadRegister<Register::X, AddressingMode::Absolute>,
      LoadRegister<Register::X, AddressingMode::AbsoluteY>,
      LoadRegister<Register::X, AddressingMode::ZeroPage>,
      LoadRegister<Register::X, AddressingMode::ZeroPageY>,
      LoadRegister<Register::Y, AddressingMode::Immediate>,
      LoadRegister<Register::Y, AddressingMode::Absolute>,
      LoadRegister<Register::Y, AddressingMode::AbsoluteX>,
      LoadRegister<Register::Y, AddressingMode::ZeroPage>,
      LoadRegister<Register::Y, AddressingMode::ZeroPageX>,
      // Stores
      StoreRegister<Register::A, AddressingMode::ZeroPage>,
      StoreRegister<Register::X, AddressingMode::ZeroPage>,
      StoreRegister<Register::Y, AddressingMode::ZeroPage>,
      StoreRegister<Register::A, AddressingMode::ZeroPageX>,
      StoreRegister<Register::X, AddressingMode::ZeroPageY>,
      StoreRegister<Register::Y, AddressingMode::ZeroPageX>,
      StoreRegister<Register::A, AddressingMode::Absolute>,
      StoreRegister<Register::X, AddressingMode::Absolute>,
      StoreRegister<Register::Y, AddressingMode::Absolute>,
      StoreRegister<Register::A, AddressingMode::AbsoluteX>,
      StoreRegister<Register::A, AddressingMode::AbsoluteY>,
      StoreRegister<Register::A, AddressingMode::IndirectX>,
      StoreRegister<Register::A, AddressingMode::IndirectY>,
      // Math
      AddWithCarry<AddressingMode::Immediate>,
      AddWithCarry<AddressingMode::ZeroPage>,
      IncrementRegister<Register::X>,
      IncrementRegister<Register::Y>,
      // ...
      TransferAccumulatorTo<Register::X>,
      TransferAccumulatorTo<Register::Y>
      >;
  // clang-format on

  [[nodiscard]] Instruction DecodeInstruction(std::span<const uint8_t> bytes) const;
  void RunInstruction(const Instruction &instr);
};

inline void CPU::Break::Apply([[maybe_unused]] CPU &cpu) const {
  // FIXME: Not elegant, but to break from the main CPU loop we throw an exception.
  //        Since we are throwing, the program_counter will not be updated by the main loop.
  //        Let's do it here.
  cpu.m_program_counter += size;
  throw NonMaskableInterrupt{};
}

template <CPU::Register REG, AddressingMode MODE> void CPU::LoadRegister<REG, MODE>::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#LDA (or #LDX,#LDY)

  if constexpr (MODE == AddressingMode::Immediate) {
    cpu.m_registers[REG] = value;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    // Zero page addressing means the memory address is in the range 0x00 to 0xFF.
    Addr addr = value & 0xFF;
    cpu.m_registers[REG] = cpu.m_ram_memory[addr];
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    // Zero page addressing with X offset means the memory address is in the range 0x00 to 0xFF, and the X register
    // is added to the zero page address.
    // If the result exceeds 0xFF, it wraps around to 0x00.
    Addr addr = (value + cpu.m_registers[Register::X]) & 0xFF;
    cpu.m_registers[REG] = cpu.m_ram_memory[addr];
  } else if constexpr (MODE == AddressingMode::ZeroPageY) {
    // Zero page addressing with Y offset means the memory address is in the range 0x00 to 0xFF, and the Y register
    // is added to the zero page address.
    // If the result exceeds 0xFF, it wraps around to 0x00.
    Addr addr = (value + cpu.m_registers[Register::Y]) & 0xFF;
    cpu.m_registers[REG] = cpu.m_ram_memory[addr];
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

    Addr target_addr = (value + cpu.m_registers[Register::X]) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr + 1) << 8 | cpu.ReadFromMemory(target_addr);
    cpu.m_registers[REG] = cpu.ReadFromMemory(real_addr);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    // Indirect indexed addressing is the most common indirection mode used on the 6502. In instruction contains the
    // zero page location of the least significant byte of 16 bit address. The Y register is dynamically added to this
    // value to generated the actual target address for operation.

    Addr real_addr = cpu.ReadFromMemory(value + 1) << 8 | cpu.ReadFromMemory(value);
    cpu.m_registers[REG] = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]);
  } else {
    TODO(fmt::format("LoadRegister<{},{}>::Apply not implemented", magic_enum::enum_name(REG),
                     magic_enum::enum_name(MODE)));
  }

  cpu.SetStatusFlag(StatusFlag::Zero, cpu.m_registers[REG] == 0);
  cpu.SetStatusFlag(StatusFlag::Negative, cpu.m_registers[REG] & 0x80);
}

template <CPU::Register REG, AddressingMode MODE> void CPU::StoreRegister<REG, MODE>::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#STA (or #STX,#STY)

  if constexpr (MODE == AddressingMode::ZeroPage) {
    // Zero page addressing means the memory address is in the range 0x00 to 0xFF.
    Addr addr = address & 0xFF;
    cpu.m_ram_memory[addr] = cpu.m_registers[REG];
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    // Zero page addressing with X offset means the memory address is in the range 0x00 to 0xFF, and the X register
    // is added to the zero page address.
    // If the result exceeds 0xFF, it wraps around to 0x00.
    Addr addr = (address + cpu.m_registers[Register::X]) & 0xFF;
    cpu.m_ram_memory[addr] = cpu.m_registers[REG];
  } else if constexpr (MODE == AddressingMode::ZeroPageY) {
    // Zero page addressing with Y offset means the memory address is in the range 0x00 to 0xFF, and the Y register
    // is added to the zero page address.
    // If the result exceeds 0xFF, it wraps around to 0x00.
    Addr addr = (address + cpu.m_registers[Register::Y]) & 0xFF;
    cpu.m_ram_memory[addr] = cpu.m_registers[REG];
  } else if constexpr (MODE == AddressingMode::Absolute) {
    // Absolute addressing means the memory address is a full 16-bit address (in LE enconding).
    cpu.WriteToMemory(address, cpu.m_registers[REG]);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    // Indexed absolute addressing means the memory address is a full 16-bit address (in LE enconding) and the X
    // register is added to the zero page address.
    Addr addr = address + cpu.m_registers[Register::X];
    cpu.WriteToMemory(addr, cpu.m_registers[REG]);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    // Indexed absolute addressing means the memory address is a full 16-bit address (in LE enconding) and the Y
    // register is added to the zero page address.
    Addr addr = address + cpu.m_registers[Register::Y];
    cpu.WriteToMemory(addr, cpu.m_registers[REG]);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    // Indexed indirect addressing is normally used in conjunction with a table of address held on zero page. The
    // address of the table is taken from the instruction and the X register added to it (with zero page wrap around) to
    // give the location of the least significant byte of the target address.

    Addr target_addr = (address + cpu.m_registers[Register::X]) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr + 1) << 8 | cpu.ReadFromMemory(target_addr);
    cpu.WriteToMemory(real_addr, cpu.m_registers[REG]);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    // Indirect indexed addressing is the most common indirection mode used on the 6502. In instruction contains the
    // zero page location of the least significant byte of 16 bit address. The Y register is dynamically added to this
    // value to generated the actual target address for operation.

    Addr real_addr = cpu.ReadFromMemory(address + 1) << 8 | cpu.ReadFromMemory(address);
    cpu.WriteToMemory(real_addr + cpu.m_registers[Register::Y], cpu.m_registers[REG]);
  } else {
    TODO(fmt::format("StoreRegister<{},{}>::Apply not implemented", magic_enum::enum_name(REG),
                     magic_enum::enum_name(MODE)));
  }

  // Note: Store instructions do not affect the processor status flags
}

template <AddressingMode MODE> void CPU::AddWithCarry<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_add = 0;
  if constexpr (MODE == AddressingMode::Immediate) {
    value_to_add = value & 0xFF; // Immediate value is already in the instruction
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_add = cpu.ReadFromMemory(value & 0xFF);
  }

  uint16_t intermediate_result = cpu.m_registers[Register::A] + value_to_add + cpu.TestStatusFlag(StatusFlag::Carry);

  // These will be useful to compute the overflow bit
  // NOTE: This is actually quite tricky, see https://www.righto.com/2012/12/the-6502-overflow-flag-explained.html for a
  // full explanation of the overflow bit
  uint8_t M = cpu.m_registers[Register::A];             // M is the original value in the accumulator
  uint8_t N = value_to_add & 0xFF;                      // N is the value being added
  uint8_t result = uint8_t(intermediate_result & 0xFF); // Result is the final value after addition

  cpu.m_registers[Register::A] = result;

  cpu.SetStatusFlag(StatusFlag::Negative, cpu.m_registers[Register::A] & 0x80);
  cpu.SetStatusFlag(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlag(StatusFlag::Carry, intermediate_result & 0x100);
  // Overflow if both operands are positive and result is negative, or both operands are negative and result is
  // positive.
  cpu.SetStatusFlag(StatusFlag::Overflow, ((M ^ result) & (N ^ result) & 0x80) != 0);
}

template <CPU::Register REG> void CPU::IncrementRegister<REG>::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#INX (or #INY)

  cpu.m_registers[REG] += 1;
  cpu.SetStatusFlag(StatusFlag::Zero, cpu.m_registers[REG] == 0);
  cpu.SetStatusFlag(StatusFlag::Negative, cpu.m_registers[REG] & 0x80);
}

template <CPU::Register REG> void CPU::TransferAccumulatorTo<REG>::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#TAX (or #TAY)

  cpu.m_registers[REG] = cpu.m_registers[Register::A];
  cpu.SetStatusFlag(StatusFlag::Zero, cpu.m_registers[REG] == 0);
  cpu.SetStatusFlag(StatusFlag::Negative, cpu.m_registers[REG] & 0x80);
}

// Constructors
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
    this->cycles = 5; // Store to indexed absolute always takes 5 cycles
  } else if constexpr (MODE == AddressingMode::IndirectX || MODE == AddressingMode::IndirectY) {
    this->cycles = 6;
  }

  address = addr;
}

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
  }

  value = _value;
}
} // namespace BNES::HW

#endif
