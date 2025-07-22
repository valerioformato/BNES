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
  case OpCode::LDA_Immediate:
    return Instruction{
        .opcode = opcode,
        .cycles = 2,
        .operands = {bytes[1]},
    };
  default:
    TODO(std::format("Implement decoding for opcode: 0x{:02X}", bytes[0]));
  }
}

void CPU::RunInstruction(const Instruction &instr) {
  switch (instr.opcode) {
  case OpCode::LDA_Immediate: {
    // Load accumulator with immediate value
    m_registers[Register::A] = instr.operands[0];
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::A] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::A] & 0x80);
    break;
  }
  default:
    TODO(std::format("Implement execution for opcode: {}", static_cast<uint8_t>(instr.opcode)));
  }

  ++m_program_counter;
}

} // namespace BNES::HW
