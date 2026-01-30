#ifndef BNES_HW_CPU_H
#define BNES_HW_CPU_H

#include "HW/Bus.h"
#include "HW/OpCodes.h"
#include "common/Types/EnumArray.h"
#include "common/Types/non_owning_ptr.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <bitset>
#include <cstdint>
#include <span>
#include <variant>

namespace BNES::Tools {
class CPUDebugger;
}

namespace BNES::HW {
class CPU {
  friend class ::BNES::Tools::CPUDebugger;
  friend class Bus;

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

  CPU() = delete;
  explicit CPU(Bus &bus) : m_bus{&bus} { m_bus->AttachCPU(this); }

  // Helper functions to inspect the state of the CPU
  [[nodiscard]] EnumArray<uint8_t, Register> Registers() const { return m_registers; };
  [[nodiscard]] uint8_t StackPointer() const { return m_stack_pointer; }
  [[nodiscard]] Addr ProgramCounter() const { return m_program_counter; }
  [[nodiscard]] std::bitset<8> StatusFlags() const { return m_status; }
  [[nodiscard]] bool TestStatusFlag(StatusFlag flag) const { return m_status.test(static_cast<size_t>(flag)); }

  void Init() {
    m_program_counter =
        ReadFromMemory(ProgramStartAddressPointer) | (ReadFromMemory(ProgramStartAddressPointer + 1) << 8);
    m_logger->debug("Init PC: 0x{:04X}", m_program_counter);
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

  std::shared_ptr<spdlog::logger> m_logger{spdlog::stdout_color_st("CPU")}; // Logger for this class

protected:
  void ProcessNMI();

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
    void Apply([[maybe_unused]] CPU &cpu) const;
  };

  struct NoOperation : DecodedInstruction {
    NoOperation() : DecodedInstruction{.size = 1, .cycles = 2} {}
    explicit NoOperation(bool undoc) : DecodedInstruction{.size = 1, .cycles = 2}, undocumented{undoc} {}
    void Apply([[maybe_unused]] CPU &cpu) const {};
    bool undocumented{false}; // true for undocumented NOP variants
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

  // undocumented load instruction...
  template <AddressingMode MODE> struct LoadAccumulatorAndX : DecodedInstruction {
    LoadAccumulatorAndX() = delete;
    explicit LoadAccumulatorAndX(uint16_t);

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

  // undocumented store instruction...
  template <AddressingMode MODE> struct StoreAccumulatorAndX : DecodedInstruction {
    StoreAccumulatorAndX() = delete;
    explicit StoreAccumulatorAndX(uint16_t);

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
    explicit SubtractWithCarry(uint16_t, bool undoc = false);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t value{0};
    bool undocumented{false}; // true for undocumented SBC variants
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
    void Apply(CPU &cpu) const;
  };

  struct PullAccumulator : DecodedInstruction {
    PullAccumulator() : DecodedInstruction{.size = 1, .cycles = 4} {}
    void Apply(CPU &cpu) const;
  };

  struct PushStatusRegister : DecodedInstruction {
    PushStatusRegister() : DecodedInstruction{.size = 1, .cycles = 3} {}
    void Apply(CPU &cpu) const;
  };

  struct PullStatusRegister : DecodedInstruction {
    PullStatusRegister() : DecodedInstruction{.size = 1, .cycles = 4} {}
    void Apply(CPU &cpu) const;
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
    void Apply(CPU &cpu) const;

    uint16_t address{0};
  };

  struct ReturnFromSubroutine : DecodedInstruction {
    ReturnFromSubroutine() : DecodedInstruction{.size = 1, .cycles = 6} {}
    void Apply(CPU &cpu) const;
  };

  struct ReturnFromInterrupt : DecodedInstruction {
    ReturnFromInterrupt() : DecodedInstruction{.size = 1, .cycles = 6} {}
    void Apply(CPU &cpu) const;
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

  // undocumented
  template <AddressingMode MODE> struct DecrementAndCompare : DecodedInstruction {
    DecrementAndCompare() = delete;
    explicit DecrementAndCompare(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t address{0};
  };

  // undocumented
  template <AddressingMode MODE> struct IncrementAndSubtract : DecodedInstruction {
    IncrementAndSubtract() = delete;
    explicit IncrementAndSubtract(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t address{0};
  };

  // undocumented
  template <AddressingMode MODE> struct ShiftLeftAndOR : DecodedInstruction {
    ShiftLeftAndOR() = delete;
    explicit ShiftLeftAndOR(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t address{0};
  };

  // undocumented
  template <AddressingMode MODE> struct RotateLeftAndAND : DecodedInstruction {
    RotateLeftAndAND() = delete;
    explicit RotateLeftAndAND(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t address{0};
  };

  // undocumented
  template <AddressingMode MODE> struct ShiftRightAndEOR : DecodedInstruction {
    ShiftRightAndEOR() = delete;
    explicit ShiftRightAndEOR(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t address{0};
  };

  // undocumented
  template <AddressingMode MODE> struct RotateRightAndAdd : DecodedInstruction {
    RotateRightAndAdd() = delete;
    explicit RotateRightAndAdd(uint16_t);

    void Apply(CPU &cpu) const;

    static constexpr AddressingMode AddrMode() { return MODE; }
    uint16_t address{0};
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
      TripleNoOperation<AddressingMode::AbsoluteX>,
      LoadAccumulatorAndX<AddressingMode::ZeroPage>,
      LoadAccumulatorAndX<AddressingMode::ZeroPageY>,
      LoadAccumulatorAndX<AddressingMode::Absolute>,
      LoadAccumulatorAndX<AddressingMode::AbsoluteY>,
      LoadAccumulatorAndX<AddressingMode::IndirectX>,
      LoadAccumulatorAndX<AddressingMode::IndirectY>,
      StoreAccumulatorAndX<AddressingMode::ZeroPage>,
      StoreAccumulatorAndX<AddressingMode::ZeroPageY>,
      StoreAccumulatorAndX<AddressingMode::IndirectX>,
      StoreAccumulatorAndX<AddressingMode::Absolute>,
      DecrementAndCompare<AddressingMode::ZeroPage>,
      DecrementAndCompare<AddressingMode::ZeroPageX>,
      DecrementAndCompare<AddressingMode::Absolute>,
      DecrementAndCompare<AddressingMode::AbsoluteX>,
      DecrementAndCompare<AddressingMode::AbsoluteY>,
      DecrementAndCompare<AddressingMode::IndirectX>,
      DecrementAndCompare<AddressingMode::IndirectY>,
      IncrementAndSubtract<AddressingMode::ZeroPage>,
      IncrementAndSubtract<AddressingMode::ZeroPageX>,
      IncrementAndSubtract<AddressingMode::Absolute>,
      IncrementAndSubtract<AddressingMode::AbsoluteX>,
      IncrementAndSubtract<AddressingMode::AbsoluteY>,
      IncrementAndSubtract<AddressingMode::IndirectX>,
      IncrementAndSubtract<AddressingMode::IndirectY>,
      ShiftLeftAndOR<AddressingMode::ZeroPage>,
      ShiftLeftAndOR<AddressingMode::ZeroPageX>,
      ShiftLeftAndOR<AddressingMode::Absolute>,
      ShiftLeftAndOR<AddressingMode::AbsoluteX>,
      ShiftLeftAndOR<AddressingMode::AbsoluteY>,
      ShiftLeftAndOR<AddressingMode::IndirectX>,
      ShiftLeftAndOR<AddressingMode::IndirectY>,
      RotateLeftAndAND<AddressingMode::ZeroPage>,
      RotateLeftAndAND<AddressingMode::ZeroPageX>,
      RotateLeftAndAND<AddressingMode::Absolute>,
      RotateLeftAndAND<AddressingMode::AbsoluteX>,
      RotateLeftAndAND<AddressingMode::AbsoluteY>,
      RotateLeftAndAND<AddressingMode::IndirectX>,
      RotateLeftAndAND<AddressingMode::IndirectY>,
      ShiftRightAndEOR<AddressingMode::ZeroPage>,
      ShiftRightAndEOR<AddressingMode::ZeroPageX>,
      ShiftRightAndEOR<AddressingMode::Absolute>,
      ShiftRightAndEOR<AddressingMode::AbsoluteX>,
      ShiftRightAndEOR<AddressingMode::AbsoluteY>,
      ShiftRightAndEOR<AddressingMode::IndirectX>,
      ShiftRightAndEOR<AddressingMode::IndirectY>,
      RotateRightAndAdd<AddressingMode::ZeroPage>,
      RotateRightAndAdd<AddressingMode::ZeroPageX>,
      RotateRightAndAdd<AddressingMode::Absolute>,
      RotateRightAndAdd<AddressingMode::AbsoluteX>,
      RotateRightAndAdd<AddressingMode::AbsoluteY>,
      RotateRightAndAdd<AddressingMode::IndirectX>,
      RotateRightAndAdd<AddressingMode::IndirectY>
      >;
  // clang-format on

  Instruction m_current_instruction{NoOperation{}};

  [[nodiscard]] Instruction DecodeInstruction(std::span<const uint8_t> bytes) const;
  void RunInstruction(Instruction &&instr);
  [[nodiscard]] std::string DisassembleInstruction(const Instruction &instr) const;
  [[nodiscard]] Instruction CurrentInstruction() const { return m_current_instruction; }
};
} // namespace BNES::HW

#endif
