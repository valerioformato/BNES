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
  enum class StatusFlag : uint8_t { Carry = 0, Zero, InterruptDisable, DecimalMode, BreakCommand, Overflow, Negative };

  static constexpr size_t STACK_MEM_SIZE = 2048;
  static constexpr size_t PROG_MEM_SIZE = 32767;

  ErrorOr<void> LoadProgram(std::vector<uint8_t> program);

  // Helper functions to inspect the state of the CPU
  [[nodiscard]] EnumArray<uint8_t, Register> Registers() const { return m_registers; };
  [[nodiscard]] uint8_t StackPointer() const { return m_stack_pointer; }
  [[nodiscard]] Addr ProgramCounter() const { return m_program_counter; }
  [[nodiscard]] std::bitset<8> StatusFlags() const { return m_status; }
  [[nodiscard]] bool TestStatusFlag(StatusFlag flag) const { return m_status.test(static_cast<size_t>(flag)); }

  [[nodiscard]] const std::array<uint8_t, PROG_MEM_SIZE> &ProgramMemory() const { return m_program_memory; }

private:
  class NonMaskableInterrupt : public std::exception {};

  void SetStatusFlag(StatusFlag flag, bool value) { m_status.set(static_cast<size_t>(flag), value); }
  void ToggleStatusFlag(StatusFlag flag) { m_status.flip(static_cast<size_t>(flag)); }

  static constexpr Addr StackBaseAddress{0x0100}; // Base address for the stack
  static constexpr Addr ProgramBaseAddress{0x8000};

  EnumArray<uint8_t, Register> m_registers{}; // Array to hold CPU registers A, X, and Y
  std::bitset<8> m_status{0x00};              // Status register (flags)
  uint8_t m_stack_pointer{0xFF};              // Stack pointer initialized to 0xFF
  Addr m_program_counter{ProgramBaseAddress}; // Program counter

  std::array<uint8_t, STACK_MEM_SIZE> m_ram_memory{}; // CPU memory (2kB)
  std::array<uint8_t, PROG_MEM_SIZE> m_program_memory{};

protected:
  // Mostly used for unit testing purposes
  uint8_t ReadFromMemory(Addr addr) const;
  void WriteToMemory(Addr addr, uint8_t value);
  void SetRegister(Register reg, uint8_t value);

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
    explicit LoadRegister(uint16_t value);

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
      IncrementRegister<Register::X>,
      IncrementRegister<Register::Y>,
      TransferAccumulatorTo<Register::X>,
      TransferAccumulatorTo<Register::Y>
      >;
  // clang-format on

  [[nodiscard]] Instruction DecodeInstruction(std::span<uint8_t> bytes) const;
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
  } else {
    TODO(fmt::format("LoadRegister<{},{}>::Apply not implemented", magic_enum::enum_name(REG),
                     magic_enum::enum_name(MODE)));
  }

  cpu.SetStatusFlag(StatusFlag::Zero, cpu.m_registers[REG] == 0);
  cpu.SetStatusFlag(StatusFlag::Negative, cpu.m_registers[REG] & 0x80);
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

} // namespace BNES::HW

#endif
