#ifndef BNES_HW_CPU_H
#define BNES_HW_CPU_H

#include "../common/Types/EnumArray.h"
#include "HW/Bus.h"
#include "HW/OpCodes.h"
#include "common/Types/non_owning_ptr.h"
#include "common/Utils.h"

#include <bitset>
#include <cstdint>
#include <span>
#include <variant>

namespace BNES::HW {
class CPU {
  friend class CPUDebugger;

public:
  using Addr = Bus::Addr;

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
  static constexpr size_t PROG_MEM_SIZE = 0x8000;

  explicit CPU(Bus &bus) : m_bus{&bus} {}

  // Helper functions to inspect the state of the CPU
  [[nodiscard]] EnumArray<uint8_t, Register> Registers() const { return m_registers; };
  [[nodiscard]] uint8_t StackPointer() const { return m_stack_pointer; }
  [[nodiscard]] Addr ProgramCounter() const { return m_program_counter; }
  [[nodiscard]] std::bitset<8> StatusFlags() const { return m_status; }
  [[nodiscard]] bool TestStatusFlag(StatusFlag flag) const { return m_status.test(static_cast<size_t>(flag)); }

  void Init() {
    m_program_counter =
        ReadFromMemory(ProgramStartAddressPointer) | (ReadFromMemory(ProgramStartAddressPointer + 1) << 8);
    spdlog::debug("Init PC: 0x{:04X}", m_program_counter);
  }

private:
  static constexpr Addr StackBaseAddress{0x0100}; // Base address for the stack
  static constexpr Addr ProgramBaseAddress{0x8000};
  static constexpr Addr ProgramStartAddressPointer{0xFFFC};

  EnumArray<uint8_t, Register> m_registers{}; // Array to hold CPU registers A, X, and Y
  std::bitset<8> m_status{0x24};              // Status register (flags)
  uint8_t m_stack_pointer{0xFD};              // Stack pointer initialized to 0xFF
  Addr m_program_counter{ProgramBaseAddress}; // Program counter
  non_owning_ptr<Bus *> m_bus;                // Memory bus

  std::array<uint8_t, STACK_MEM_SIZE> m_ram_memory{}; // CPU memory (2kB)
  std::array<uint8_t, PROG_MEM_SIZE> m_program_memory{};

protected:
  class NonMaskableInterrupt : public std::exception {};

  [[nodiscard]] uint8_t ReadFromMemory(Addr addr) const;
  void WriteToMemory(Addr addr, uint8_t value);

  // Mostly used for unit testing purposes
  void SetRegister(Register reg, uint8_t value);
  void SetStatusFlagValue(StatusFlag flag, bool value) { m_status.set(static_cast<size_t>(flag), value); }
  void ToggleStatusFlag(StatusFlag flag) { m_status.flip(static_cast<size_t>(flag)); }

  void SetProgramStartAddress(Addr addr) { m_program_counter = addr; }

  const Bus &GetBus() const { return *m_bus; }

public:
  struct DecodedInstruction {
    uint8_t size;
    uint8_t cycles;
  };

  struct Break : DecodedInstruction {
    Break() : DecodedInstruction{.size = 1, .cycles = 7} {}
    inline void Apply([[maybe_unused]] CPU &cpu) const;
  };

  struct NoOperation : DecodedInstruction {
    NoOperation() : DecodedInstruction{.size = 1, .cycles = 2} {}
    void Apply([[maybe_unused]] CPU &cpu) const {};
  };

  template <AddressingMode MODE> struct DoubleNoOperation : DecodedInstruction {
    DoubleNoOperation() = delete;
    explicit DoubleNoOperation(uint16_t);

    void Apply([[maybe_unused]] CPU &cpu) const {};

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t value{0};
  };

  template <AddressingMode MODE> struct TripleNoOperation : DecodedInstruction {
    TripleNoOperation() = delete;
    explicit TripleNoOperation(uint16_t addr) : DecodedInstruction{.size = 3, .cycles = 4}, value{addr} {}

    void Apply([[maybe_unused]] CPU &cpu) const {};

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t value{0};
  };

  template <Register REG, AddressingMode MODE> struct LoadRegister : DecodedInstruction {
    LoadRegister() = delete;
    explicit LoadRegister(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t value{0};
  };

  template <Register REG, AddressingMode MODE> struct StoreRegister : DecodedInstruction {
    StoreRegister() = delete;
    explicit StoreRegister(uint16_t addr);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t address{0};
  };

  template <AddressingMode MODE> struct AddWithCarry : DecodedInstruction {
    AddWithCarry() = delete;
    explicit AddWithCarry(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t value{0};
  };

  template <AddressingMode MODE> struct SubtractWithCarry : DecodedInstruction {
    SubtractWithCarry() = delete;
    explicit SubtractWithCarry(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t value{0};
  };

  template <AddressingMode MODE> struct LogicalAND : DecodedInstruction {
    LogicalAND() = delete;
    explicit LogicalAND(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t value{0};
  };

  template <AddressingMode MODE> struct ShiftLeft : DecodedInstruction {
    ShiftLeft() = delete;
    explicit ShiftLeft(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t address{0};
  };

  template <AddressingMode MODE> struct ShiftRight : DecodedInstruction {
    ShiftRight() = delete;
    explicit ShiftRight(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t address{0};
  };

  template <AddressingMode MODE> struct RotateRight : DecodedInstruction {
    RotateRight() = delete;
    explicit RotateRight(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t address{0};
  };

  template <AddressingMode MODE> struct RotateLeft : DecodedInstruction {
    RotateLeft() = delete;
    explicit RotateLeft(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t address{0};
  };

  template <Register REG> struct IncrementRegister : DecodedInstruction {
    IncrementRegister() : DecodedInstruction{.size = 1, .cycles = 2} {}
    void Apply(CPU &cpu) const;
  };

  template <AddressingMode MODE> struct Increment : DecodedInstruction {
    Increment() = delete;
    explicit Increment(uint16_t addr);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t address{0};
  };

  template <Register REG> struct DecrementRegister : DecodedInstruction {
    DecrementRegister() : DecodedInstruction{.size = 1, .cycles = 2} {}
    void Apply(CPU &cpu) const;
  };

  template <AddressingMode MODE> struct Decrement : DecodedInstruction {
    Decrement() = delete;
    explicit Decrement(uint16_t addr);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t address{0};
  };

  template <Register REG, AddressingMode MODE> struct CompareRegister : DecodedInstruction {
    CompareRegister() = delete;
    explicit CompareRegister(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t value{0};
  };

  template <Register SRCREG, Register DSTREG> struct TransferRegisterTo : DecodedInstruction {
    TransferRegisterTo() : DecodedInstruction{.size = 1, .cycles = 2} {}
    void Apply(CPU &cpu) const;
  };

  struct TransferStackPointerToX : DecodedInstruction {
    TransferStackPointerToX() : DecodedInstruction{.size = 1, .cycles = 2} {};
    void Apply(CPU &cpu) const;
  };

  struct TransferXToStackPointer : DecodedInstruction {
    TransferXToStackPointer() : DecodedInstruction{.size = 1, .cycles = 2} {};
    void Apply(CPU &cpu) const;
  };
  struct PushAccumulator : DecodedInstruction {
    PushAccumulator() : DecodedInstruction{.size = 1, .cycles = 3} {}
    inline void Apply(CPU &cpu) const;
  };

  struct PullAccumulator : DecodedInstruction {
    PullAccumulator() : DecodedInstruction{.size = 1, .cycles = 4} {}
    inline void Apply(CPU &cpu) const;
  };

  struct PushStatusRegister : DecodedInstruction {
    PushStatusRegister() : DecodedInstruction{.size = 1, .cycles = 3} {}
    inline void Apply(CPU &cpu) const;
  };

  struct PullStatusRegister : DecodedInstruction {
    PullStatusRegister() : DecodedInstruction{.size = 1, .cycles = 4} {}
    inline void Apply(CPU &cpu) const;
  };

  template <Conditional COND> struct Branch : DecodedInstruction {
    Branch() = delete;
    explicit Branch(int8_t offset_) : DecodedInstruction{.size = 2, .cycles = 2}, offset(offset_) {}
    void Apply(CPU &cpu);

    int8_t offset{0}; // in branch instruction the offset is always signed
  };

  template <AddressingMode MODE> struct Jump : DecodedInstruction {
    Jump() = delete;
    explicit Jump(uint16_t addr);
    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t address{0};
  };

  struct JumpToSubroutine : DecodedInstruction {
    JumpToSubroutine() = delete;
    explicit JumpToSubroutine(uint16_t addr) : DecodedInstruction{.size = 3, .cycles = 6}, address(addr) {}
    inline void Apply(CPU &cpu) const;

    uint16_t address{0};
  };

  struct ReturnFromSubroutine : DecodedInstruction {
    ReturnFromSubroutine() : DecodedInstruction{.size = 1, .cycles = 6} {}
    inline void Apply(CPU &cpu) const;
  };

  struct ReturnFromInterrupt : DecodedInstruction {
    ReturnFromInterrupt() : DecodedInstruction{.size = 1, .cycles = 6} {}
    inline void Apply(CPU &cpu) const;
  };

  template <AddressingMode MODE> struct BitTest : DecodedInstruction {
    BitTest() = delete;
    explicit BitTest(uint16_t addr);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t address{0};
  };

  template <StatusFlag FLAG> struct ClearStatusFlag : DecodedInstruction {
    ClearStatusFlag() : DecodedInstruction{.size = 1, .cycles = 2} {}
    void Apply(CPU &cpu) const;
  };

  template <StatusFlag FLAG> struct SetStatusFlag : DecodedInstruction {
    SetStatusFlag() : DecodedInstruction{.size = 1, .cycles = 2} {}
    void Apply(CPU &cpu) const;
  };

  template <AddressingMode MODE> struct ExclusiveOR : DecodedInstruction {
    ExclusiveOR() = delete;
    explicit ExclusiveOR(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t value{0};
  };

  template <AddressingMode MODE> struct BitwiseOR : DecodedInstruction {
    BitwiseOR() = delete;
    explicit BitwiseOR(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t value{0};
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
      AddWithCarry<AddressingMode::ZeroPageX>,
      AddWithCarry<AddressingMode::Absolute>,
      AddWithCarry<AddressingMode::AbsoluteX>,
      AddWithCarry<AddressingMode::AbsoluteY>,
      AddWithCarry<AddressingMode::IndirectX>,
      AddWithCarry<AddressingMode::IndirectY>,
      SubtractWithCarry<AddressingMode::Immediate>,
      SubtractWithCarry<AddressingMode::ZeroPage>,
      SubtractWithCarry<AddressingMode::ZeroPageX>,
      SubtractWithCarry<AddressingMode::Absolute>,
      SubtractWithCarry<AddressingMode::AbsoluteX>,
      SubtractWithCarry<AddressingMode::AbsoluteY>,
      SubtractWithCarry<AddressingMode::IndirectX>,
      SubtractWithCarry<AddressingMode::IndirectY>,
      LogicalAND<AddressingMode::Immediate>,
      LogicalAND<AddressingMode::ZeroPage>,
      LogicalAND<AddressingMode::ZeroPageX>,
      LogicalAND<AddressingMode::Absolute>,
      LogicalAND<AddressingMode::AbsoluteX>,
      LogicalAND<AddressingMode::AbsoluteY>,
      LogicalAND<AddressingMode::IndirectX>,
      LogicalAND<AddressingMode::IndirectY>,
      ShiftLeft<AddressingMode::Accumulator>,
      ShiftLeft<AddressingMode::ZeroPage>,
      ShiftLeft<AddressingMode::ZeroPageX>,
      ShiftLeft<AddressingMode::Absolute>,
      ShiftLeft<AddressingMode::AbsoluteX>,
      ShiftRight<AddressingMode::Accumulator>,
      ShiftRight<AddressingMode::ZeroPage>,
      ShiftRight<AddressingMode::ZeroPageX>,
      ShiftRight<AddressingMode::Absolute>,
      ShiftRight<AddressingMode::AbsoluteX>,
      RotateRight<AddressingMode::Accumulator>,
      RotateRight<AddressingMode::ZeroPage>,
      RotateRight<AddressingMode::ZeroPageX>,
      RotateRight<AddressingMode::Absolute>,
      RotateRight<AddressingMode::AbsoluteX>,
      RotateLeft<AddressingMode::Accumulator>,
      RotateLeft<AddressingMode::ZeroPage>,
      RotateLeft<AddressingMode::ZeroPageX>,
      RotateLeft<AddressingMode::Absolute>,
      RotateLeft<AddressingMode::AbsoluteX>,
      IncrementRegister<Register::X>,
      IncrementRegister<Register::Y>,
      Increment<AddressingMode::ZeroPage>,
      Increment<AddressingMode::ZeroPageX>,
      Increment<AddressingMode::Absolute>,
      Increment<AddressingMode::AbsoluteX>,
      DecrementRegister<Register::X>,
      DecrementRegister<Register::Y>,
      Decrement<AddressingMode::ZeroPage>,
      Decrement<AddressingMode::ZeroPageX>,
      Decrement<AddressingMode::Absolute>,
      Decrement<AddressingMode::AbsoluteX>,
      ExclusiveOR<AddressingMode::Immediate>,
      ExclusiveOR<AddressingMode::ZeroPage>,
      ExclusiveOR<AddressingMode::ZeroPageX>,
      ExclusiveOR<AddressingMode::Absolute>,
      ExclusiveOR<AddressingMode::AbsoluteX>,
      ExclusiveOR<AddressingMode::AbsoluteY>,
      ExclusiveOR<AddressingMode::IndirectX>,
      ExclusiveOR<AddressingMode::IndirectY>,
      BitwiseOR<AddressingMode::Immediate>,
      BitwiseOR<AddressingMode::ZeroPage>,
      BitwiseOR<AddressingMode::ZeroPageX>,
      BitwiseOR<AddressingMode::Absolute>,
      BitwiseOR<AddressingMode::AbsoluteX>,
      BitwiseOR<AddressingMode::AbsoluteY>,
      BitwiseOR<AddressingMode::IndirectX>,
      BitwiseOR<AddressingMode::IndirectY>,
  // Branch
      Branch<Conditional::Equal>,
      Branch<Conditional::NotEqual>,
      Branch<Conditional::CarrySet>,
      Branch<Conditional::CarryClear>,
      Branch<Conditional::Minus>,
      Branch<Conditional::Positive>,
      Branch<Conditional::OverflowSet>,
      Branch<Conditional::OverflowClear>,
      Jump<AddressingMode::Absolute>,
      Jump<AddressingMode::Indirect>,
      JumpToSubroutine,
      ReturnFromSubroutine,
      ReturnFromInterrupt,
      // ...
      BitTest<AddressingMode::ZeroPage>,
      BitTest<AddressingMode::Absolute>,
      ClearStatusFlag<StatusFlag::Carry>,
      ClearStatusFlag<StatusFlag::DecimalMode>,
      ClearStatusFlag<StatusFlag::InterruptDisable>,
      ClearStatusFlag<StatusFlag::Overflow>,
      CompareRegister<Register::X, AddressingMode::Immediate>,
      CompareRegister<Register::X, AddressingMode::ZeroPage>,
      CompareRegister<Register::X, AddressingMode::Absolute>,
      CompareRegister<Register::Y, AddressingMode::Immediate>,
      CompareRegister<Register::Y, AddressingMode::ZeroPage>,
      CompareRegister<Register::Y, AddressingMode::Absolute>,
      CompareRegister<Register::A, AddressingMode::Immediate>,
      CompareRegister<Register::A, AddressingMode::ZeroPage>,
      CompareRegister<Register::A, AddressingMode::ZeroPageX>,
      CompareRegister<Register::A, AddressingMode::Absolute>,
      CompareRegister<Register::A, AddressingMode::AbsoluteX>,
      CompareRegister<Register::A, AddressingMode::AbsoluteY>,
      CompareRegister<Register::A, AddressingMode::IndirectX>,
      CompareRegister<Register::A, AddressingMode::IndirectY>,
      SetStatusFlag<StatusFlag::Carry>,
      SetStatusFlag<StatusFlag::DecimalMode>,
      SetStatusFlag<StatusFlag::InterruptDisable>,
      TransferRegisterTo<Register::A, Register::X>,
      TransferRegisterTo<Register::A, Register::Y>,
      TransferRegisterTo<Register::X, Register::A>,
      TransferRegisterTo<Register::Y, Register::A>,
      TransferStackPointerToX,
      TransferXToStackPointer,
      // Stack
      PushAccumulator,
      PullAccumulator,
      PushStatusRegister,
      PullStatusRegister,
      //
      NoOperation,
      DoubleNoOperation<AddressingMode::Immediate>,
      DoubleNoOperation<AddressingMode::ZeroPage>,
      DoubleNoOperation<AddressingMode::ZeroPageX>,
      TripleNoOperation<AddressingMode::Absolute>,
      TripleNoOperation<AddressingMode::AbsoluteX>
      >;
  // clang-format on

  Instruction m_current_instruction{NoOperation{}};

  [[nodiscard]] Instruction DecodeInstruction(std::span<const uint8_t> bytes) const;
  void RunInstruction(Instruction &&instr);
  [[nodiscard]] std::string DisassembleInstruction(const Instruction &instr) const;
  [[nodiscard]] Instruction CurrentInstruction() const { return m_current_instruction; }
};

inline void CPU::Break::Apply([[maybe_unused]] CPU &cpu) const {
  // FIXME: Not elegant, but to break from the main CPU loop we throw an exception.
  //        Since we are throwing, the program_counter will not be updated by the main loop.
  //        Let's do it here.
  cpu.m_program_counter += size;
  throw NonMaskableInterrupt{};
}

inline void CPU::PushAccumulator::Apply(CPU &cpu) const {
  cpu.WriteToMemory(StackBaseAddress + cpu.m_stack_pointer, cpu.m_registers[Register::A]);
  cpu.m_stack_pointer--;
}

inline void CPU::PullAccumulator::Apply(CPU &cpu) const {
  cpu.m_stack_pointer++;
  cpu.m_registers[Register::A] = cpu.ReadFromMemory(StackBaseAddress + cpu.m_stack_pointer);

  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, (cpu.m_registers[Register::A] & 0x80) != 0);
}

inline void CPU::PushStatusRegister::Apply(CPU &cpu) const {
  uint8_t status_word = cpu.m_status.to_ulong() | 0x30;
  cpu.WriteToMemory(StackBaseAddress + cpu.m_stack_pointer, status_word);
  cpu.m_stack_pointer--;
}

inline void CPU::PullStatusRegister::Apply(CPU &cpu) const {
  cpu.m_stack_pointer++;
  cpu.m_status = (cpu.ReadFromMemory(StackBaseAddress + cpu.m_stack_pointer) & 0xEF) | 0x20;

  // TODO: Note that the effect of changing I is delayed one instruction because the flag is changed after IRQ is
  //       polled, delaying the effect until IRQ is polled in the next instruction like with CLI and SEI.
}

template <Conditional COND> inline void CPU::Branch<COND>::Apply(CPU &cpu) {
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

template <AddressingMode MODE> void CPU::Jump<MODE>::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#JMP

  Addr target_address{0};
  if constexpr (MODE == AddressingMode::Absolute) {
    target_address = address;
  } else if constexpr (MODE == AddressingMode::Indirect) {
    // Indirect jump is used to jump to a subroutine or a specific address.
    // NOTE: 6502 bug - if the indirect vector falls on a page boundary (e.g. $xxFF),
    // it fetches LSB from $xxFF as expected but MSB from $xx00 (not $xx00+$0100).
    // This is a well-known hardware bug in the original 6502.
    Addr low_byte = cpu.ReadFromMemory(address);
    Addr high_byte;
    if ((address & 0xFF) == 0xFF) {
      // Page boundary: wrap high byte read to start of same page
      high_byte = cpu.ReadFromMemory(address & 0xFF00);
    } else {
      high_byte = cpu.ReadFromMemory(address + 1);
    }
    target_address = (high_byte << 8) | low_byte;
  } else {
    TODO(fmt::format("Jump<{}>::Apply not implemented", magic_enum::enum_name(MODE)));
  }

  cpu.m_program_counter = target_address;
}

inline void CPU::JumpToSubroutine::Apply(CPU &cpu) const {
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

inline void CPU::ReturnFromSubroutine::Apply(CPU &cpu) const {
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

inline void CPU::ReturnFromInterrupt::Apply(CPU &cpu) const {
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

template <CPU::Register REG, AddressingMode MODE> void CPU::StoreRegister<REG, MODE>::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#STA (or #STX,#STY)

  if constexpr (MODE == AddressingMode::ZeroPage) {
    // Zero page addressing means the memory address is in the range 0x00 to 0xFF.
    Addr addr = address & 0xFF;
    cpu.WriteToMemory(addr, cpu.m_registers[REG]);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    // Zero page addressing with X offset means the memory address is in the range 0x00 to 0xFF, and the X register
    // is added to the zero page address.
    // If the result exceeds 0xFF, it wraps around to 0x00.
    Addr addr = (address + cpu.m_registers[Register::X]) & 0xFF;
    cpu.WriteToMemory(addr, cpu.m_registers[REG]);
  } else if constexpr (MODE == AddressingMode::ZeroPageY) {
    // Zero page addressing with Y offset means the memory address is in the range 0x00 to 0xFF, and the Y register
    // is added to the zero page address.
    // If the result exceeds 0xFF, it wraps around to 0x00.
    Addr addr = (address + cpu.m_registers[Register::Y]) & 0xFF;
    cpu.WriteToMemory(addr, cpu.m_registers[REG]);
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

    Addr target_addr_low = (address + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (address + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    cpu.WriteToMemory(real_addr, cpu.m_registers[REG]);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    // Indirect indexed addressing is the most common indirection mode used on the 6502. In instruction contains the
    // zero page location of the least significant byte of 16 bit address. The Y register is dynamically added to this
    // value to generated the actual target address for operation.

    Addr target_addr_low = address & 0xFF;
    Addr target_addr_high = (address + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
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

  // These will be useful to compute the overflow bit
  // NOTE: This is actually quite tricky, see https://www.righto.com/2012/12/the-6502-overflow-flag-explained.html for a
  // full explanation of the overflow bit
  uint8_t M = cpu.m_registers[Register::A];             // M is the original value in the accumulator
  uint8_t N = value_to_add & 0xFF;                      // N is the value being added
  uint8_t result = uint8_t(intermediate_result & 0xFF); // Result is the final value after addition

  cpu.m_registers[Register::A] = result;

  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::A] & 0x80);
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Carry, intermediate_result & 0x100);
  // Overflow if both operands are positive and result is negative, or both operands are negative and result is
  // positive.
  cpu.SetStatusFlagValue(StatusFlag::Overflow, ((M ^ result) & (N ^ result) & 0x80) != 0);
}

template <AddressingMode MODE> void CPU::SubtractWithCarry<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_add = 0;
  if constexpr (MODE == AddressingMode::Immediate) {
    value_to_add = value & 0xFF; // Immediate value is already in the instruction
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

  // These will be useful to compute the overflow bit
  // NOTE: This is actually quite tricky, see https://www.righto.com/2012/12/the-6502-overflow-flag-explained.html for a
  // full explanation of the overflow bit
  // For SBC, we convert to addition: A - B = A + (~B) + 1 (with carry acting as the +1)
  uint8_t M = cpu.m_registers[Register::A];             // M is the original value in the accumulator
  uint8_t N = ~(value_to_add & 0xFF);                   // N is the one's complement of the value being subtracted
  uint8_t result = uint8_t(intermediate_result & 0xFF); // Result is the final value after subtraction

  cpu.m_registers[Register::A] = result;

  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::A] & 0x80);
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Carry, !(intermediate_result & 0x100)); // Inverted for subtraction
  // Overflow formula is the same as ADC when using one's complement
  cpu.SetStatusFlagValue(StatusFlag::Overflow, ((M ^ result) & (N ^ result) & 0x80) != 0);
}

template <AddressingMode MODE> void CPU::LogicalAND<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_and = 0;
  if constexpr (MODE == AddressingMode::Immediate) {
    value_to_and = value & 0xFF; // Immediate value is already in the instruction
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

template <AddressingMode MODE> void CPU::RotateRight<MODE>::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#ROR

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

template <AddressingMode MODE> void CPU::RotateLeft<MODE>::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#ROL

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

template <CPU::Register REG> void CPU::IncrementRegister<REG>::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#INX (or #INY)

  cpu.m_registers[REG] += 1;
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[REG] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[REG] & 0x80);
}

template <CPU::Register REG> void CPU::DecrementRegister<REG>::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#DEX (or #DEY)

  cpu.m_registers[REG] -= 1;
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[REG] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[REG] & 0x80);
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

template <CPU::Register SRCREG, CPU::Register DSTREG>
void CPU::TransferRegisterTo<SRCREG, DSTREG>::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#TAX (or #TAY)

  cpu.m_registers[DSTREG] = cpu.m_registers[SRCREG];
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[DSTREG] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[DSTREG] & 0x80);
}

inline void CPU::TransferStackPointerToX::Apply(CPU &cpu) const {
  cpu.m_registers[Register::X] = cpu.m_stack_pointer;

  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::X] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::X] & 0x80);
}

inline void CPU::TransferXToStackPointer::Apply(CPU &cpu) const { cpu.m_stack_pointer = cpu.m_registers[Register::X]; }

template <CPU::Register REG, AddressingMode MODE> void CPU::CompareRegister<REG, MODE>::Apply(CPU &cpu) const {
  // See https://www.nesdev.org/obelisk-6502-guide/reference.html#CMP (or #CPX or #CPY)
  uint8_t value_to_compare{0};
  if constexpr (MODE == AddressingMode::Immediate) {
    value_to_compare = value & 0xFF; // Immediate value is already in the instruction
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    value_to_compare = cpu.ReadFromMemory(value & 0xFF);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    // Zero Page,X addressing adds the X register to the zero page address with wraparound
    Addr effective_addr = (value + cpu.m_registers[Register::X]) & 0xFF;
    value_to_compare = cpu.ReadFromMemory(effective_addr);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    value_to_compare = cpu.ReadFromMemory(value);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    // Absolute,X addressing adds the X register to the absolute address
    Addr effective_addr = value + cpu.m_registers[Register::X];
    value_to_compare = cpu.ReadFromMemory(effective_addr);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    // Absolute,Y addressing adds the Y register to the absolute address
    Addr effective_addr = value + cpu.m_registers[Register::Y];
    value_to_compare = cpu.ReadFromMemory(effective_addr);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    // Indexed indirect addressing is normally used in conjunction with a table of address held on zero page. The
    // address of the table is taken from the instruction and the X register added to it (with zero page wrap around) to
    // give the location of the least significant byte of the target address.

    Addr target_addr_low = (value + cpu.m_registers[Register::X]) & 0xFF;
    Addr target_addr_high = (value + cpu.m_registers[Register::X] + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_compare = cpu.ReadFromMemory(real_addr);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    // Indirect indexed addressing is the most common indirection mode used on the 6502. In instruction contains the
    // zero page location of the least significant byte of 16 bit address. The Y register is dynamically added to this
    // value to generated the actual target address for operation.

    Addr target_addr_low = value & 0xFF;
    Addr target_addr_high = (value + 1) & 0xFF;
    Addr real_addr = cpu.ReadFromMemory(target_addr_high) << 8 | cpu.ReadFromMemory(target_addr_low);
    value_to_compare = cpu.ReadFromMemory(real_addr + cpu.m_registers[Register::Y]);
  }

  cpu.SetStatusFlagValue(StatusFlag::Carry, cpu.m_registers[REG] >= value_to_compare);
  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[REG] == value_to_compare);
  cpu.SetStatusFlagValue(StatusFlag::Negative, (cpu.m_registers[REG] - value_to_compare) & 0x80);
}

template <CPU::StatusFlag FLAG> void CPU::ClearStatusFlag<FLAG>::Apply(CPU &cpu) const {
  cpu.SetStatusFlagValue(FLAG, false);
}

template <CPU::StatusFlag FLAG> void CPU::SetStatusFlag<FLAG>::Apply(CPU &cpu) const {
  cpu.SetStatusFlagValue(FLAG, true);
}

template <AddressingMode MODE> void CPU::ExclusiveOR<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_or{0};

  if constexpr (MODE == AddressingMode::Immediate) {
    value_to_or = value & 0xFF; // Immediate value is already in the instruction
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

template <AddressingMode MODE> void CPU::BitwiseOR<MODE>::Apply(CPU &cpu) const {
  uint8_t value_to_or{0};

  if constexpr (MODE == AddressingMode::Immediate) {
    value_to_or = value & 0xFF; // Immediate value is already in the instruction
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

  cpu.m_registers[Register::A] |= value_to_or;

  cpu.SetStatusFlagValue(StatusFlag::Zero, cpu.m_registers[Register::A] == 0);
  cpu.SetStatusFlagValue(StatusFlag::Negative, cpu.m_registers[Register::A] & 0x80);
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
  } else {
    std::unreachable();
  }

  value = _value;
}

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

template <AddressingMode MODE> CPU::SubtractWithCarry<MODE>::SubtractWithCarry(uint16_t _value) {
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

template <CPU::Register REG, AddressingMode MODE> CPU::CompareRegister<REG, MODE>::CompareRegister(uint16_t _value) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::Immediate) {
    this->cycles = 2;
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 3;
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::Absolute) {
    this->size = 3;
    this->cycles = 4;
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    this->size = 3;
    this->cycles = 4; // +1 if page crossed
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    this->size = 3;
    this->cycles = 4; // +1 if page crossed
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    this->cycles = 6;
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    this->cycles = 5; // +1 if page crossed
  } else {
    std::unreachable();
  }

  value = _value;
}

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

template <AddressingMode MODE> CPU::BitTest<MODE>::BitTest(uint16_t addr) {
  this->size = 2;

  if constexpr (MODE == AddressingMode::ZeroPage) {
    this->cycles = 3;
  } else if constexpr (MODE == AddressingMode::Absolute) {
    this->size = 3;
    this->cycles = 4;
  } else {
    std::unreachable();
  }

  address = addr;
}

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
  } else {
    std::unreachable();
  }

  value = addr;
}

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
  } else {
    std::unreachable();
  }

  value = addr;
}

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
} // namespace BNES::HW

#endif
