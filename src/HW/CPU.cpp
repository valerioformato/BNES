#include "HW/CPU.h"
#include "common/Types/overloaded.h"

namespace BNES::HW {
CPU::Instruction CPU::DecodeInstruction(std::span<const uint8_t> bytes) const {
  assert(!bytes.empty());

  auto opcode = static_cast<OpCode>(bytes[0]);

  switch (opcode) {
  case OpCode::Break:
    return Break{};
  // Load instructions
  case OpCode::LDA_Immediate:
    return LoadRegister<Register::A, AddressingMode::Immediate>{bytes[1]};
  case OpCode::LDX_Immediate:
    return LoadRegister<Register::X, AddressingMode::Immediate>{bytes[1]};
  case OpCode::LDY_Immediate:
    return LoadRegister<Register::Y, AddressingMode::Immediate>{bytes[1]};
  case OpCode::LDA_ZeroPage:
    return LoadRegister<Register::A, AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::LDX_ZeroPage:
    return LoadRegister<Register::X, AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::LDY_ZeroPage:
    return LoadRegister<Register::Y, AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::LDA_ZeroPageX:
    return LoadRegister<Register::A, AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::LDX_ZeroPageY:
    return LoadRegister<Register::X, AddressingMode::ZeroPageY>{bytes[1]};
  case OpCode::LDY_ZeroPageX:
    return LoadRegister<Register::Y, AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::LDA_Absolute:
    return LoadRegister<Register::A, AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::LDX_Absolute:
    return LoadRegister<Register::X, AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::LDY_Absolute:
    return LoadRegister<Register::Y, AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::LDA_AbsoluteX:
    return LoadRegister<Register::A, AddressingMode::AbsoluteX>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::LDA_AbsoluteY:
    return LoadRegister<Register::A, AddressingMode::AbsoluteY>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::LDX_AbsoluteY:
    return LoadRegister<Register::X, AddressingMode::AbsoluteY>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::LDY_AbsoluteX:
    return LoadRegister<Register::Y, AddressingMode::AbsoluteX>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::LDA_IndirectX:
    return LoadRegister<Register::A, AddressingMode::IndirectX>{bytes[1]};
  case OpCode::LDA_IndirectY:
    return LoadRegister<Register::A, AddressingMode::IndirectY>{bytes[1]};
  // Store instructions
  case OpCode::STA_ZeroPage:
    return StoreRegister<Register::A, AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::STX_ZeroPage:
    return StoreRegister<Register::X, AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::STY_ZeroPage:
    return StoreRegister<Register::Y, AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::STA_ZeroPageX:
    return StoreRegister<Register::A, AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::STX_ZeroPageY:
    return StoreRegister<Register::X, AddressingMode::ZeroPageY>{bytes[1]};
  case OpCode::STY_ZeroPageX:
    return StoreRegister<Register::Y, AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::STA_Absolute:
    return StoreRegister<Register::A, AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::STX_Absolute:
    return StoreRegister<Register::X, AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::STY_Absolute:
    return StoreRegister<Register::Y, AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::STA_AbsoluteX:
    return StoreRegister<Register::A, AddressingMode::AbsoluteX>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::STA_AbsoluteY:
    return StoreRegister<Register::A, AddressingMode::AbsoluteY>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::STA_IndirectX:
    return StoreRegister<Register::A, AddressingMode::IndirectX>{bytes[1]};
  case OpCode::STA_IndirectY:
    return StoreRegister<Register::A, AddressingMode::IndirectY>{bytes[1]};
  // Transfer instructions
  case OpCode::TAX:
    return TransferRegisterTo<Register::A, Register::X>{};
  case OpCode::TAY:
    return TransferRegisterTo<Register::A, Register::Y>{};
  case OpCode::TXA:
    return TransferRegisterTo<Register::X, Register::A>{};
  case OpCode::TYA:
    return TransferRegisterTo<Register::Y, Register::A>{};
  case OpCode::TXS:
    return TransferXToStackPointer{};
  case OpCode::TSX:
    return TransferStackPointerToX{};
    // Stack instructions
  case OpCode::PHA:
    return PushAccumulator{};
  case OpCode::PLA:
    return PullAccumulator{};
  case OpCode::PHP:
    return PushStatusRegister{};
  case OpCode::PLP:
    return PullStatusRegister{};
    // Math instructions
  case OpCode::ADC_Immediate:
    return AddWithCarry<AddressingMode::Immediate>{bytes[1]};
  case OpCode::ADC_ZeroPage:
    return AddWithCarry<AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::ADC_ZeroPageX:
    return AddWithCarry<AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::ADC_Absolute:
    return AddWithCarry<AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::ADC_AbsoluteX:
    return AddWithCarry<AddressingMode::AbsoluteX>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::ADC_AbsoluteY:
    return AddWithCarry<AddressingMode::AbsoluteY>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::ADC_IndirectX:
    return AddWithCarry<AddressingMode::IndirectX>{bytes[1]};
  case OpCode::ADC_IndirectY:
    return AddWithCarry<AddressingMode::IndirectY>{bytes[1]};
  case OpCode::SBC_Immediate:
    return SubtractWithCarry<AddressingMode::Immediate>{bytes[1]};
  case OpCode::SBC_ZeroPage:
    return SubtractWithCarry<AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::SBC_ZeroPageX:
    return SubtractWithCarry<AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::SBC_Absolute:
    return SubtractWithCarry<AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::SBC_AbsoluteX:
    return SubtractWithCarry<AddressingMode::AbsoluteX>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::SBC_AbsoluteY:
    return SubtractWithCarry<AddressingMode::AbsoluteY>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::SBC_IndirectX:
    return SubtractWithCarry<AddressingMode::IndirectX>{bytes[1]};
  case OpCode::SBC_IndirectY:
    return SubtractWithCarry<AddressingMode::IndirectY>{bytes[1]};
  case OpCode::AND_Immediate:
    return LogicalAND<AddressingMode::Immediate>{bytes[1]};
  case OpCode::AND_ZeroPage:
    return LogicalAND<AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::AND_ZeroPageX:
    return LogicalAND<AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::AND_Absolute:
    return LogicalAND<AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::AND_AbsoluteX:
    return LogicalAND<AddressingMode::AbsoluteX>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::AND_AbsoluteY:
    return LogicalAND<AddressingMode::AbsoluteY>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::AND_IndirectX:
    return LogicalAND<AddressingMode::IndirectX>{bytes[1]};
  case OpCode::AND_IndirectY:
    return LogicalAND<AddressingMode::IndirectY>{bytes[1]};
  case OpCode::ASL_Accumulator:
    return ShiftLeft<AddressingMode::Accumulator>{0};
  case OpCode::ASL_ZeroPage:
    return ShiftLeft<AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::ASL_ZeroPageX:
    return ShiftLeft<AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::ASL_Absolute:
    return ShiftLeft<AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::ASL_AbsoluteX:
    return ShiftLeft<AddressingMode::AbsoluteX>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::LSR_Accumulator:
    return ShiftRight<AddressingMode::Accumulator>{0};
  case OpCode::LSR_ZeroPage:
    return ShiftRight<AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::LSR_ZeroPageX:
    return ShiftRight<AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::LSR_Absolute:
    return ShiftRight<AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::LSR_AbsoluteX:
    return ShiftRight<AddressingMode::AbsoluteX>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::ROR_Accumulator:
    return RotateRight<AddressingMode::Accumulator>{0};
  case OpCode::ROR_ZeroPage:
    return RotateRight<AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::ROR_ZeroPageX:
    return RotateRight<AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::ROR_Absolute:
    return RotateRight<AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::ROR_AbsoluteX:
    return RotateRight<AddressingMode::AbsoluteX>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::ROL_Accumulator:
    return RotateLeft<AddressingMode::Accumulator>{0};
  case OpCode::ROL_ZeroPage:
    return RotateLeft<AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::ROL_ZeroPageX:
    return RotateLeft<AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::ROL_Absolute:
    return RotateLeft<AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::ROL_AbsoluteX:
    return RotateLeft<AddressingMode::AbsoluteX>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::INC_ZeroPage:
    return Increment<AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::INC_ZeroPageX:
    return Increment<AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::INC_Absolute:
    return Increment<AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::INC_AbsoluteX:
    return Increment<AddressingMode::AbsoluteX>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::INX:
    return IncrementRegister<Register::X>{};
  case OpCode::INY:
    return IncrementRegister<Register::Y>{};
  case OpCode::DEX:
    return DecrementRegister<Register::X>{};
  case OpCode::DEY:
    return DecrementRegister<Register::Y>{};
  case OpCode::DEC_ZeroPage:
    return Decrement<AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::DEC_ZeroPageX:
    return Decrement<AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::DEC_Absolute:
    return Decrement<AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::DEC_AbsoluteX:
    return Decrement<AddressingMode::AbsoluteX>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::EOR_Immediate:
    return ExclusiveOR<AddressingMode::Immediate>{bytes[1]};
  case OpCode ::EOR_ZeroPage:
    return ExclusiveOR<AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::EOR_ZeroPageX:
    return ExclusiveOR<AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::EOR_Absolute:
    return ExclusiveOR<AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::EOR_AbsoluteX:
    return ExclusiveOR<AddressingMode::AbsoluteX>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::EOR_AbsoluteY:
    return ExclusiveOR<AddressingMode::AbsoluteY>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::EOR_IndirectX:
    return ExclusiveOR<AddressingMode::IndirectX>{bytes[1]};
  case OpCode::EOR_IndirectY:
    return ExclusiveOR<AddressingMode::IndirectY>{bytes[1]};
  case OpCode::ORA_Immediate:
    return BitwiseOR<AddressingMode::Immediate>{bytes[1]};
  case OpCode ::ORA_ZeroPage:
    return BitwiseOR<AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::ORA_ZeroPageX:
    return BitwiseOR<AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::ORA_Absolute:
    return BitwiseOR<AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::ORA_AbsoluteX:
    return BitwiseOR<AddressingMode::AbsoluteX>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::ORA_AbsoluteY:
    return BitwiseOR<AddressingMode::AbsoluteY>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::ORA_IndirectX:
    return BitwiseOR<AddressingMode::IndirectX>{bytes[1]};
  case OpCode::ORA_IndirectY:
    return BitwiseOR<AddressingMode::IndirectY>{bytes[1]};
    // Branch instructions
  case OpCode::BEQ:
    return Branch<Conditional::Equal>{int8_t(bytes[1])};
  case OpCode::BNE:
    return Branch<Conditional::NotEqual>{int8_t(bytes[1])};
  case OpCode::BCC:
    return Branch<Conditional::CarryClear>{int8_t(bytes[1])};
  case OpCode::BCS:
    return Branch<Conditional::CarrySet>{int8_t(bytes[1])};
  case OpCode::BMI:
    return Branch<Conditional::Minus>{int8_t(bytes[1])};
  case OpCode::BPL:
    return Branch<Conditional::Positive>{int8_t(bytes[1])};
  case OpCode::BVC:
    return Branch<Conditional::OverflowClear>{int8_t(bytes[1])};
  case OpCode::BVS:
    return Branch<Conditional::OverflowSet>{int8_t(bytes[1])};
  case OpCode::JMP_Absolute:
    return Jump<AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::JMP_Indirect:
    return Jump<AddressingMode::Indirect>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::JSR:
    return JumpToSubroutine{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::RTS:
    return ReturnFromSubroutine{};
  case OpCode::RTI:
    return ReturnFromInterrupt{};
  // ...
  case OpCode::BIT_ZeroPage:
    return BitTest<AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::BIT_Absolute:
    return BitTest<AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::CLC:
    return ClearStatusFlag<StatusFlag::Carry>{};
  case OpCode::CLD:
    return ClearStatusFlag<StatusFlag::DecimalMode>{};
  case OpCode::CLI:
    return ClearStatusFlag<StatusFlag::InterruptDisable>{};
  case OpCode::CLV:
    return ClearStatusFlag<StatusFlag::Overflow>{};
  case OpCode::CPX_Immediate:
    return CompareRegister<Register::X, AddressingMode::Immediate>{bytes[1]};
  case OpCode::CPX_ZeroPage:
    return CompareRegister<Register::X, AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::CPX_Absolute:
    return CompareRegister<Register::X, AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::CPY_Immediate:
    return CompareRegister<Register::Y, AddressingMode::Immediate>{bytes[1]};
  case OpCode::CPY_ZeroPage:
    return CompareRegister<Register::Y, AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::CPY_Absolute:
    return CompareRegister<Register::Y, AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::CMP_Immediate:
    return CompareRegister<Register::A, AddressingMode::Immediate>{bytes[1]};
  case OpCode::CMP_ZeroPage:
    return CompareRegister<Register::A, AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::CMP_ZeroPageX:
    return CompareRegister<Register::A, AddressingMode::ZeroPageX>{bytes[1]};
  case OpCode::CMP_Absolute:
    return CompareRegister<Register::A, AddressingMode::Absolute>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::CMP_AbsoluteX:
    return CompareRegister<Register::A, AddressingMode::AbsoluteX>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::CMP_AbsoluteY:
    return CompareRegister<Register::A, AddressingMode::AbsoluteY>{uint16_t(bytes[2] << 8 | bytes[1])};
  case OpCode::CMP_IndirectX:
    return CompareRegister<Register::A, AddressingMode::IndirectX>{bytes[1]};
  case OpCode::CMP_IndirectY:
    return CompareRegister<Register::A, AddressingMode::IndirectY>{bytes[1]};
  case OpCode::SEC:
    return SetStatusFlag<StatusFlag::Carry>{};
  case OpCode::SED:
    return SetStatusFlag<StatusFlag::DecimalMode>{};
  case OpCode::SEI:
    return SetStatusFlag<StatusFlag::InterruptDisable>{};
  case OpCode::NOP:
    return NoOperation{};
  case OpCode::DOP_Immediate_80:
  case OpCode::DOP_Immediate_82:
  case OpCode::DOP_Immediate_89:
  case OpCode::DOP_Immediate_C2:
  case OpCode::DOP_Immediate_E2:
    return DoubleNoOperation<AddressingMode::Immediate>{bytes[1]};
  case OpCode::DOP_ZeroPage_04:
  case OpCode::DOP_ZeroPage_44:
  case OpCode::DOP_ZeroPage_64:
    return DoubleNoOperation<AddressingMode::ZeroPage>{bytes[1]};
  case OpCode::DOP_ZeroPageX_14:
  case OpCode::DOP_ZeroPageX_34:
  case OpCode::DOP_ZeroPageX_54:
  case OpCode::DOP_ZeroPageX_74:
  case OpCode::DOP_ZeroPageX_D4:
  case OpCode::DOP_ZeroPageX_F4:
    return DoubleNoOperation<AddressingMode::ZeroPageX>{bytes[1]};
  default:
    spdlog::error("Unknown opcode: 0x{:02X}", bytes[0]);
    TODO(std::format("Implement decoding for opcode: 0x{:02X}", bytes[0]));
  }

  // We should never get here
  std::unreachable();
}

void CPU::RunInstruction(Instruction &&instr) {
  m_current_instruction = instr;
  std::visit<void>(
      [this](auto &instruction) {
        // Run the instruction on the CPU
        instruction.Apply(*this);

        if constexpr (!(std::is_same_v<std::decay_t<decltype(instruction)>, JumpToSubroutine> ||
                        std::is_same_v<std::decay_t<decltype(instruction)>, ReturnFromSubroutine> ||
                        std::is_same_v<std::decay_t<decltype(instruction)>, ReturnFromInterrupt> ||
                        std::is_same_v<std::decay_t<decltype(instruction)>, Jump<AddressingMode::Absolute>> ||
                        std::is_same_v<std::decay_t<decltype(instruction)>, Jump<AddressingMode::Indirect>>)) {
          // Advance the program counter by the size of the instruction
          // We avoid this for jump instructions, since they directly modify the program counter
          m_program_counter += instruction.size;
        }
      },
      instr);
}

// Helper function to format operands in proper 6502 assembly notation
namespace {
template <AddressingMode MODE> std::string FormatOperand(uint16_t value) {
  if constexpr (MODE == AddressingMode::Immediate) {
    return fmt::format("#${:02X}", value);
  } else if constexpr (MODE == AddressingMode::ZeroPage) {
    return fmt::format("${:02X}", value);
  } else if constexpr (MODE == AddressingMode::ZeroPageX) {
    return fmt::format("${:02X},X", value);
  } else if constexpr (MODE == AddressingMode::ZeroPageY) {
    return fmt::format("${:02X},Y", value);
  } else if constexpr (MODE == AddressingMode::Absolute) {
    return fmt::format("${:04X}", value);
  } else if constexpr (MODE == AddressingMode::AbsoluteX) {
    return fmt::format("${:04X},X", value);
  } else if constexpr (MODE == AddressingMode::AbsoluteY) {
    return fmt::format("${:04X},Y", value);
  } else if constexpr (MODE == AddressingMode::Indirect) {
    return fmt::format("(${:04X})", value);
  } else if constexpr (MODE == AddressingMode::IndirectX) {
    return fmt::format("(${:02X},X)", value);
  } else if constexpr (MODE == AddressingMode::IndirectY) {
    return fmt::format("(${:02X}),Y", value);
  } else if constexpr (MODE == AddressingMode::Accumulator) {
    return "A";
  } else {
    return "???";
  }
}
} // namespace

std::string CPU::DisassembleInstruction(const Instruction &instr) const {
  const uint16_t currentPC = m_program_counter; // Capture PC value for use in lambdas
  return std::visit(Utils::overloaded{
                        [](const Break &) -> std::string { return "BRK"; },
                        []<Register REG, AddressingMode MODE>(const LoadRegister<REG, MODE> _inst) {
                          return fmt::format("LD{} {}", magic_enum::enum_name(REG), FormatOperand<MODE>(_inst.value));
                        },
                        []<Register REG, AddressingMode MODE>(const StoreRegister<REG, MODE> _inst) {
                          return fmt::format("ST{} {}", magic_enum::enum_name(REG), FormatOperand<MODE>(_inst.address));
                        },
                        []<Register SRC, Register DST>(const TransferRegisterTo<SRC, DST> &) {
                          return fmt::format("T{}{}", magic_enum::enum_name(SRC), magic_enum::enum_name(DST));
                        },
                        [](const TransferStackPointerToX &) -> std::string { return "TSX"; },
                        [](const TransferXToStackPointer &) -> std::string { return "TXS"; },
                        [](const PushAccumulator &) -> std::string { return "PHA"; },
                        [](const PullAccumulator &) -> std::string { return "PLA"; },
                        [](const PushStatusRegister &) -> std::string { return "PHP"; },
                        [](const PullStatusRegister &) -> std::string { return "PLP"; },
                        []<AddressingMode MODE>(const AddWithCarry<MODE> _inst) {
                          return fmt::format("ADC {}", FormatOperand<MODE>(_inst.value));
                        },
                        []<AddressingMode MODE>(const SubtractWithCarry<MODE> _inst) {
                          return fmt::format("SBC {}", FormatOperand<MODE>(_inst.value));
                        },
                        []<AddressingMode MODE>(const LogicalAND<MODE> _inst) {
                          return fmt::format("AND {}", FormatOperand<MODE>(_inst.value));
                        },
                        []<AddressingMode MODE>(const ShiftLeft<MODE> _inst) {
                          if constexpr (MODE == AddressingMode::Accumulator) {
                            return std::string("ASL A");
                          } else {
                            return fmt::format("ASL {}", FormatOperand<MODE>(_inst.address));
                          }
                        },
                        []<AddressingMode MODE>(const ShiftRight<MODE> _inst) {
                          if constexpr (MODE == AddressingMode::Accumulator) {
                            return std::string("LSR A");
                          } else {
                            return fmt::format("LSR {}", FormatOperand<MODE>(_inst.address));
                          }
                        },
                        []<AddressingMode MODE>(const RotateRight<MODE> _inst) {
                          if constexpr (MODE == AddressingMode::Accumulator) {
                            return std::string("ROR A");
                          } else {
                            return fmt::format("ROR {}", FormatOperand<MODE>(_inst.address));
                          }
                        },
                        []<AddressingMode MODE>(const RotateLeft<MODE> _inst) {
                          if constexpr (MODE == AddressingMode::Accumulator) {
                            return std::string("ROL A");
                          } else {
                            return fmt::format("ROL {}", FormatOperand<MODE>(_inst.address));
                          }
                        },
                        []<AddressingMode MODE>(const Increment<MODE> _inst) {
                          return fmt::format("INC {}", FormatOperand<MODE>(_inst.address));
                        },
                        [](const IncrementRegister<Register::X> &) -> std::string { return "INX"; },
                        [](const IncrementRegister<Register::Y> &) -> std::string { return "INY"; },
                        [](const DecrementRegister<Register::X> &) -> std::string { return "DEX"; },
                        [](const DecrementRegister<Register::Y> &) -> std::string { return "DEY"; },
                        []<AddressingMode MODE>(const Decrement<MODE> _inst) {
                          return fmt::format("DEC {}", FormatOperand<MODE>(_inst.address));
                        },
                        []<AddressingMode MODE>(const ExclusiveOR<MODE> _inst) {
                          return fmt::format("EOR {}", FormatOperand<MODE>(_inst.value));
                        },
                        []<AddressingMode MODE>(const BitwiseOR<MODE> _inst) {
                          return fmt::format("ORA {}", FormatOperand<MODE>(_inst.value));
                        },
                        [currentPC]<Conditional COND>(const Branch<COND> _inst) -> std::string {
                          // Calculate target address: PC + 2 (instruction size) + signed offset
                          uint16_t target = currentPC + 2 + static_cast<int8_t>(_inst.offset);
                          switch (COND) {
                          case Conditional::Equal:
                            return fmt::format("BEQ ${:04X}", target);
                          case Conditional::NotEqual:
                            return fmt::format("BNE ${:04X}", target);
                          case Conditional::CarrySet:
                            return fmt::format("BCS ${:04X}", target);
                          case Conditional::CarryClear:
                            return fmt::format("BCC ${:04X}", target);
                          case Conditional::Minus:
                            return fmt::format("BMI ${:04X}", target);
                          case Conditional::Positive:
                            return fmt::format("BPL ${:04X}", target);
                          case Conditional::OverflowSet:
                            return fmt::format("BVS ${:04X}", target);
                          case Conditional::OverflowClear:
                            return fmt::format("BVC ${:04X}", target);
                          default:
                            return "BR?"; // Unknown condition
                          }
                        },
                        []<AddressingMode MODE>(const Jump<MODE> _inst) {
                          return fmt::format("JMP {}", FormatOperand<MODE>(_inst.address));
                        },
                        [](const JumpToSubroutine &inst) { return fmt::format("JSR ${:04X}", inst.address); },
                        [](const ReturnFromSubroutine &) -> std::string { return "RTS"; },
                        [](const ReturnFromInterrupt &) -> std::string { return "RTI"; },
                        []<AddressingMode MODE>(const BitTest<MODE> _inst) {
                          return fmt::format("BIT {}", FormatOperand<MODE>(_inst.address));
                        },
                        []<StatusFlag FLAG>(const ClearStatusFlag<FLAG> &) -> std::string {
                          switch (FLAG) {
                          case StatusFlag::Carry:
                            return "CLC";
                          case StatusFlag::DecimalMode:
                            return "CLD";
                          case StatusFlag::InterruptDisable:
                            return "CLI";
                          case StatusFlag::Overflow:
                            return "CLV";
                          default:
                            return "CL?"; // Unknown flag
                          }
                        },
                        []<StatusFlag FLAG>(const SetStatusFlag<FLAG> &) -> std::string {
                          switch (FLAG) {
                          case StatusFlag::Carry:
                            return "SEC";
                          case StatusFlag::DecimalMode:
                            return "SED";
                          case StatusFlag::InterruptDisable:
                            return "SEI";
                          default:
                            return "SE?"; // Unknown flag
                          }
                        },
                        []<Register REG, AddressingMode MODE>(const CompareRegister<REG, MODE> _inst) {
                          // Special case: Accumulator comparison uses CMP, not CPA
                          std::string mnemonic =
                              (REG == Register::A) ? "CMP" : fmt::format("CP{}", magic_enum::enum_name(REG));
                          return fmt::format("{} {}", mnemonic, FormatOperand<MODE>(_inst.value));
                        },
                        [](const NoOperation &) -> std::string { return "NOP"; },
                        []<AddressingMode MODE>(const DoubleNoOperation<MODE> _inst) -> std::string {
                          return fmt::format("*NOP {}", FormatOperand<MODE>(_inst.value));
                        },
                        [](const auto &) -> std::string { return "Unimplemented disassembly"; },
                    },
                    instr);
}

uint8_t CPU::ReadFromMemory(Addr addr) const { return m_bus->Read(addr); }

void CPU::WriteToMemory(Addr addr, uint8_t value) { return m_bus->Write(addr, value); }

void CPU::SetRegister(Register reg, uint8_t value) {
  m_registers[reg] = value;
  SetStatusFlagValue(StatusFlag::Zero, value == 0);
  SetStatusFlagValue(StatusFlag::Negative, value & 0x80);
}

} // namespace BNES::HW
