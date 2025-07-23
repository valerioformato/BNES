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
  case OpCode::LDX_Immediate:
  case OpCode::LDY_Immediate:
    return Instruction{
        .opcode = opcode,
        .cycles = 2,
        .size = 2,
        .operands = {bytes[1]},
    };
  case OpCode::LDA_ZeroPage:
  case OpCode::LDX_ZeroPage:
  case OpCode::LDY_ZeroPage:
    return Instruction{
        .opcode = opcode,
        .cycles = 3,
        .size = 2,
        .operands = {bytes[1]},
    };
  case OpCode::LDA_ZeroPageX:
  case OpCode::LDX_ZeroPageY:
  case OpCode::LDY_ZeroPageX:
    return Instruction{
        .opcode = opcode,
        .cycles = 4,
        .size = 2,
        .operands = {bytes[1]},
    };
  case OpCode::LDA_Absolute:
  case OpCode::LDX_Absolute:
  case OpCode::LDY_Absolute:
    return Instruction{
        .opcode = opcode,
        .cycles = 4,
        .size = 3,
        .operands = {bytes[1], bytes[2]},
    };
  case OpCode::LDA_AbsoluteX:
  case OpCode::LDA_AbsoluteY:
  case OpCode::LDX_AbsoluteY:
  case OpCode::LDY_AbsoluteX:
    return Instruction{
        .opcode = opcode,
        .cycles = 4,
        .size = 3,
        .operands = {bytes[1], bytes[2]},
    };
  case OpCode::TAX:
  case OpCode::INX:
    return Instruction{
        .opcode = opcode,
        .cycles = 2,
        .size = 1,
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
  case OpCode::LDY_Immediate: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDY
    // Loads a byte of memory into the Y register setting the zero and negative flags as appropriate.

    m_registers[Register::Y] = instr.operands[0];
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::Y] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::Y] & 0x80);
    break;
  }
  case OpCode::LDA_ZeroPage: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDA
    // Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate.
    // Zero page addressing means the memory address is in the range 0x00 to 0xFF.

    Addr addr = instr.operands[0];

    m_registers[Register::A] = m_ram_memory[addr];
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::A] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::A] & 0x80);
    break;
  }
  case OpCode::LDX_ZeroPage: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDX
    // Loads a byte of memory into the X register setting the zero and negative flags as appropriate.
    // Zero page addressing means the memory address is in the range 0x00 to 0xFF.

    Addr addr = instr.operands[0];

    m_registers[Register::X] = m_ram_memory[addr];
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::X] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::X] & 0x80);
    break;
  }
  case OpCode::LDY_ZeroPage: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDY
    // Loads a byte of memory into the Y register setting the zero and negative flags as appropriate.
    // Zero page addressing means the memory address is in the range 0x00 to 0xFF.

    Addr addr = instr.operands[0];

    m_registers[Register::Y] = m_ram_memory[addr];
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::Y] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::Y] & 0x80);
    break;
  }
  case OpCode::LDA_ZeroPageX: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDA
    // Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate.
    // Zero page addressing with X offset means the memory address is in the range 0x00 to 0xFF, and the X register
    // is added to the zero page address.
    // If the result exceeds 0xFF, it wraps around to 0x00.

    Addr addr = (instr.operands[0] + m_registers[Register::X]) & 0xFF;

    m_registers[Register::A] = m_ram_memory[addr];
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::A] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::A] & 0x80);
    break;
  }
  case OpCode::LDX_ZeroPageY: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDX
    // Loads a byte of memory into the X register setting the zero and negative flags as appropriate.
    // Zero page addressing with Y offset means the memory address is in the range 0x00 to 0xFF, and the Y register
    // is added to the zero page address.
    // If the result exceeds 0xFF, it wraps around to 0x00.

    Addr addr = (instr.operands[0] + m_registers[Register::Y]) & 0xFF;

    m_registers[Register::X] = m_ram_memory[addr];
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::X] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::X] & 0x80);
    break;
  }
  case OpCode::LDY_ZeroPageX: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDY
    // Loads a byte of memory into the Y register setting the zero and negative flags as appropriate.
    // Zero page addressing with X offset means the memory address is in the range 0x00 to 0xFF, and the X register
    // is added to the zero page address.
    // If the result exceeds 0xFF, it wraps around to 0x00.

    Addr addr = (instr.operands[0] + m_registers[Register::X]) & 0xFF;

    m_registers[Register::Y] = m_ram_memory[addr];
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::Y] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::Y] & 0x80);
    break;
  }
  case OpCode::LDA_Absolute: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDA
    // Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate.
    // Absolute addressing means the memory address is a full 16-bit address (in LE enconding).

    Addr addr = (instr.operands[1] << 8) | instr.operands[0];

    m_registers[Register::A] = ReadFromMemory(addr);
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::A] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::A] & 0x80);
    break;
  }
  case OpCode::LDX_Absolute: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDX
    // Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate.
    // Absolute addressing means the memory address is a full 16-bit address (in LE enconding).

    Addr addr = (instr.operands[1] << 8) | instr.operands[0];

    m_registers[Register::X] = ReadFromMemory(addr);
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::X] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::X] & 0x80);
    break;
  }
  case OpCode::LDY_Absolute: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDY
    // Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate.
    // Absolute addressing means the memory address is a full 16-bit address (in LE enconding).

    Addr addr = (instr.operands[1] << 8) | instr.operands[0];

    m_registers[Register::Y] = ReadFromMemory(addr);
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::Y] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::Y] & 0x80);
    break;
  }
  case OpCode::LDA_AbsoluteX: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDA
    // Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate.
    // Absolute addressing means the memory address is a full 16-bit address (in LE enconding) and the X register
    // is added to the zero page address.

    Addr addr = (instr.operands[1] << 8) | instr.operands[0];
    addr += m_registers[Register::X];

    m_registers[Register::A] = ReadFromMemory(addr);
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::A] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::A] & 0x80);
    break;
  }
  case OpCode::LDA_AbsoluteY: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDA
    // Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate.
    // Absolute addressing means the memory address is a full 16-bit address (in LE enconding) and the Y register
    // is added to the zero page address.

    Addr addr = (instr.operands[1] << 8) | instr.operands[0];
    addr += m_registers[Register::Y];

    m_registers[Register::A] = ReadFromMemory(addr);
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::A] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::A] & 0x80);
    break;
  }
  case OpCode::LDX_AbsoluteY: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDA
    // Loads a byte of memory into the X register setting the zero and negative flags as appropriate.
    // Absolute addressing means the memory address is a full 16-bit address (in LE enconding) and the Y register
    // is added to the zero page address.

    Addr addr = (instr.operands[1] << 8) | instr.operands[0];
    addr += m_registers[Register::Y];

    m_registers[Register::X] = ReadFromMemory(addr);
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::X] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::X] & 0x80);
    break;
  }
  case OpCode::LDY_AbsoluteX: {
    // https://www.nesdev.org/obelisk-6502-guide/reference.html#LDA
    // Loads a byte of memory into the Y register setting the zero and negative flags as appropriate.
    // Absolute addressing means the memory address is a full 16-bit address (in LE enconding) and the X register
    // is added to the zero page address.

    Addr addr = (instr.operands[1] << 8) | instr.operands[0];
    addr += m_registers[Register::X];

    m_registers[Register::Y] = ReadFromMemory(addr);
    SetStatusFlag(StatusFlag::Zero, m_registers[Register::Y] == 0);
    SetStatusFlag(StatusFlag::Negative, m_registers[Register::Y] & 0x80);
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
