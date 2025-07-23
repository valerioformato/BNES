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
    return Instruction{
        .opcode = opcode,
        .cycles = 7, // BRK takes 7 cycles
        .size = 1,   // BRK is a single byte instruction
        .operands = {},
    };
  case OpCode::LDA_Immediate:
    return Instruction{
        .opcode = opcode,
        .cycles = 2,
        .size = 2, // LDA immediate is a two-byte instruction (opcode + operand)
        .operands = {bytes[1]},
    };
  case OpCode::LDX_Immediate:
    return Instruction{
        .opcode = opcode,
        .cycles = 2,
        .size = 2, // LDX immediate is a two-byte instruction (opcode + operand)
        .operands = {bytes[1]},
    };
  case OpCode::TAX:
    return Instruction{
        .opcode = opcode,
        .cycles = 2,
        .size = 1, // TAX is a single byte instruction
        .operands = {},
    };
  case OpCode::INX:
    return Instruction{
        .opcode = opcode,
        .cycles = 2,
        .size = 1, // INX is a single byte instruction
        .operands = {},
    };
  default:
    TODO(std::format("Implement decoding for opcode: 0x{:02X}", bytes[0]));
  }
}

void CPU::RunInstruction(const Instruction &instr) {
  switch (instr.opcode) {
  case OpCode::Break: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#BRK
    // The BRK instruction forces the generation of an interrupt request. The program counter and processor status are
    // pushed on the stack then the IRQ interrupt vector at $FFFE/F is loaded into the PC and the break flag in the
    // status set to one.

    ++m_program_counter;

    // FIXME: Not elegant, but to break from the main CPU loop we throw an exception.
    throw NonMaskableInterrupt{};
  }
  case OpCode::LDA_Immediate: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDA
    // Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate.

    m_registers[Register::A] = instr.operands[0];
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::A] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::A] & 0x80);
    break;
  }
  case OpCode::LDX_Immediate: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDX
    // Loads a byte of memory into the X register setting the zero and negative flags as appropriate.

    m_registers[Register::X] = instr.operands[0];
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::X] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::X] & 0x80);
    break;
  }
  case OpCode::TAX: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#TAX
    // Copies the current contents of the accumulator into the X register and sets the zero and negative flags as
    // appropriate.

    m_registers[Register::X] = m_registers[Register::A];
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::X] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::X] & 0x80);
    break;
  }
  case OpCode::INX: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#INX
    // Adds one to the X register setting the zero and negative flags as appropriate.

    ++m_registers[Register::X];
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::X] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::X] & 0x80);
    break;
  }
  default:
    TODO(std::format("Implement execution for opcode: 0x{:02X}", static_cast<uint8_t>(instr.opcode)));
  }

  // Advance the program counter by the size of the instruction
  m_program_counter += instr.size;
}

} // namespace BNES::HW
