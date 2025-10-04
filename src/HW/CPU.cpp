#include "HW/CPU.h"

namespace BNES::HW {
ErrorOr<void> CPU::LoadProgram(std::span<const uint8_t> program) {
  if (program.size() > m_program_memory.size()) {
    return make_error(std::make_error_code(std::errc::not_enough_memory), "Program too large to fit in memory");
  }
  std::ranges::copy(program, m_program_memory.begin());

  return {};
}

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
  // ...
  case OpCode::TAX:
    return TransferRegisterTo<Register::A, Register::X>{};
  case OpCode::TAY:
    return TransferRegisterTo<Register::A, Register::Y>{};
  case OpCode::TXA:
    return TransferRegisterTo<Register::X, Register::A>{};
  case OpCode::TYA:
    return TransferRegisterTo<Register::Y, Register::A>{};
    // Stack instructions
  case OpCode::PHA:
    return PushAccumulator{};
  case OpCode::PLA:
    return PullAccumulator{};
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
  case OpCode::SEC:
    return SetStatusFlag<StatusFlag::Carry>{};
  case OpCode::SED:
    return SetStatusFlag<StatusFlag::DecimalMode>{};
  case OpCode::SEI:
    return SetStatusFlag<StatusFlag::InterruptDisable>{};
  case OpCode::NOP:
    return NoOperation{};
  default:
    TODO(std::format("Implement decoding for opcode: 0x{:02X}", bytes[0]));
  }

  // We should never get here
  std::unreachable();
}

void CPU::RunInstruction(Instruction &&instr) {
  std::visit<void>(
      [this](auto &instruction) {
        // Run the instruction on the CPU
        instruction.Apply(*this);

        if constexpr (!(std::is_same_v<std::decay_t<decltype(instruction)>, JumpToSubroutine> ||
                        std::is_same_v<std::decay_t<decltype(instruction)>, ReturnFromSubroutine> ||
                        std::is_same_v<std::decay_t<decltype(instruction)>, Jump<AddressingMode::Absolute>> ||
                        std::is_same_v<std::decay_t<decltype(instruction)>, Jump<AddressingMode::Indirect>>)) {
          // Advance the program counter by the size of the instruction
          // We avoid this for jump instructions, since they directly modify the program counter
          m_program_counter += instruction.size;
        }
      },
      instr);
}

std::string CPU::DisassembleInstruction(const Instruction &instr) {
  return std::visit(
      Utils::overloaded{
          [](const Break &) -> std::string { return "BRK"; },
          []<Register REG, AddressingMode MODE>(const LoadRegister<REG, MODE> _inst) {
            return fmt::format("LD{} {} {:02X}", magic_enum::enum_name(REG), magic_enum::enum_name(MODE), _inst.value);
          },
          []<Register REG, AddressingMode MODE>(const StoreRegister<REG, MODE> _inst) {
            return fmt::format("ST{} {} {:02X}", magic_enum::enum_name(REG), magic_enum::enum_name(MODE),
                               _inst.address);
          },
          []<Register SRC, Register DST>(const TransferRegisterTo<SRC, DST> &) {
            return fmt::format("T{}{}", magic_enum::enum_name(SRC), magic_enum::enum_name(DST));
          },
          [](const PushAccumulator &) -> std::string { return "PHA"; },
          [](const PullAccumulator &) -> std::string { return "PLA"; },
          []<AddressingMode MODE>(const AddWithCarry<MODE> _inst) {
            return fmt::format("ADC {} {:02X}", magic_enum::enum_name(MODE), _inst.value);
          },
          []<AddressingMode MODE>(const LogicalAND<MODE> _inst) {
            return fmt::format("AND {} {:02X}", magic_enum::enum_name(MODE), _inst.value);
          },
          []<AddressingMode MODE>(const ShiftLeft<MODE> _inst) {
            return fmt::format("ASL {} {:02X}", magic_enum::enum_name(MODE), _inst.address);
          },
          []<AddressingMode MODE>(const ShiftRight<MODE> _inst) {
            return fmt::format("LSR {} {:02X}", magic_enum::enum_name(MODE), _inst.address);
          },
          []<AddressingMode MODE>(const Increment<MODE> _inst) {
            return fmt::format("INC {} {:02X}", magic_enum::enum_name(MODE), _inst.address);
          },
          [](const IncrementRegister<Register::X> &) -> std::string { return "INX"; },
          [](const IncrementRegister<Register::Y> &) -> std::string { return "INY"; },
          [](const DecrementRegister<Register::X> &) -> std::string { return "DEX"; },
          [](const DecrementRegister<Register::Y> &) -> std::string { return "DEY"; },
          []<AddressingMode MODE>(const Decrement<MODE> _inst) {
            return fmt::format("DEC {} {:02X}", magic_enum::enum_name(MODE), _inst.address);
          },
          []<AddressingMode MODE>(const ExclusiveOR<MODE> _inst) {
            return fmt::format("EOR {} {:02X}", magic_enum::enum_name(MODE), _inst.value);
          },
          []<Conditional COND>(const Branch<COND> _inst) {
            return fmt::format("B{} {:02X}", magic_enum::enum_name(COND), _inst.offset);
          },
          []<AddressingMode MODE>(const Jump<MODE> _inst) {
            return fmt::format("JMP {} {:02X}", magic_enum::enum_name(MODE), _inst.address);
          },
          [](const JumpToSubroutine &inst) { return fmt::format("JSR {:02X}", inst.address); },
          [](const ReturnFromSubroutine &) -> std::string { return "RTS"; },
          []<AddressingMode MODE>(const BitTest<MODE> _inst) {
            return fmt::format("BIT {} {:02X}", magic_enum::enum_name(MODE), _inst.address);
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
            return fmt::format("CP{} {} {:02X}", magic_enum::enum_name(REG), magic_enum::enum_name(MODE), _inst.value);
          },
          [](const NoOperation &) -> std::string { return "NOP"; },
          [](const auto &) -> std::string { return "Unimplemented disassembly"; },
      },
      instr);
}

uint8_t CPU::ReadFromMemory(Addr addr) const {
  if (addr < m_ram_memory.size()) {
    return m_ram_memory[addr];
  }

  if (addr >= m_ram_memory.size() && addr < ProgramBaseAddress) {
    TODO("implement the rest of the memory map for the CPU");
  }

  if (addr > ProgramBaseAddress && addr < ProgramBaseAddress + m_program_memory.size()) {
    return m_program_memory[addr - ProgramBaseAddress];
  }

  throw std::out_of_range("Address out of range for RAM or program memory");
}

void CPU::WriteToMemory(Addr addr, uint8_t value) {
  if (addr < m_ram_memory.size()) {
    m_ram_memory[addr] = value;
  } else {
    throw std::out_of_range("Address out of range for RAM memory");
  }
}

void CPU::SetRegister(Register reg, uint8_t value) {
  m_registers[reg] = value;
  SetStatusFlagValue(StatusFlag::Zero, value == 0);
  SetStatusFlagValue(StatusFlag::Negative, value & 0x80);
}

} // namespace BNES::HW
