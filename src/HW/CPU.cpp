#include "HW/CPU.h"

namespace BNES::HW {
ErrorOr<void> CPU::LoadProgram(std::vector<uint8_t> program) {
  if (program.size() > m_program_memory.size()) {
    return make_error(std::make_error_code(std::errc::not_enough_memory), "Program too large to fit in memory");
  }
  std::ranges::copy(program, m_program_memory.begin());

  return {};
}

CPU::Instruction CPU::DecodeInstruction(std::span<uint8_t> bytes) const {
  assert(!bytes.empty());

  auto opcode = static_cast<OpCode>(bytes[0]);

  switch (opcode) {
  case OpCode::Break:
    return Break{};
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
  case OpCode::TAX:
    return TransferAccumulatorTo<Register::X>{};
  case OpCode::TAY:
    return TransferAccumulatorTo<Register::Y>{};
  case OpCode::INX:
    return IncrementRegister<Register::X>{};
  case OpCode::INY:
    return IncrementRegister<Register::Y>{};
  default:
    TODO(std::format("Implement decoding for opcode: 0x{:02X}", bytes[0]));
  }

  // We should never get here
  std::unreachable();
}

void CPU::RunInstruction(const Instruction &instr) {
  std::visit(
      [this](const auto &instruction) {
        // Run the instruction on the CPU
        instruction.Apply(*this);

        // Advance the program counter by the size of the instruction
        m_program_counter += instruction.size;
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
  SetStatusFlag(StatusFlag::Zero, value == 0);
  SetStatusFlag(StatusFlag::Negative, value & 0x80);
}

} // namespace BNES::HW
