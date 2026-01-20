//
// Created by Valerio Formato on 09-Jan-26.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

SCENARIO("6502 instruction disassembly tests (math ops)", "[Disassemble][Math]") {
  GIVEN("A freshly initialized cpu") {
    Bus bus;
    CPU cpu{bus};

    // INX/INY instructions
    WHEN("We disassemble an INX instruction") {
      auto instruction = CPU::IncrementRegister<CPU::Register::X>{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'INX'") { REQUIRE(disassembly == "INX"); }
    }

    WHEN("We disassemble an INY instruction") {
      auto instruction = CPU::IncrementRegister<CPU::Register::Y>{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'INY'") { REQUIRE(disassembly == "INY"); }
    }

    // DEX/DEY instructions
    WHEN("We disassemble a DEX instruction") {
      auto instruction = CPU::DecrementRegister<CPU::Register::X>{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'DEX'") { REQUIRE(disassembly == "DEX"); }
    }

    WHEN("We disassemble a DEY instruction") {
      auto instruction = CPU::DecrementRegister<CPU::Register::Y>{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'DEY'") { REQUIRE(disassembly == "DEY"); }
    }

    // ADC instructions
    WHEN("We disassemble an ADC immediate instruction") {
      auto instruction = CPU::AddWithCarry<AddressingMode::Immediate>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ADC #$42'") { REQUIRE(disassembly == "ADC #$42"); }
    }

    WHEN("We disassemble an ADC zero-page instruction") {
      auto instruction = CPU::AddWithCarry<AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ADC $42'") { REQUIRE(disassembly == "ADC $42"); }
    }

    WHEN("We disassemble an ADC zero-page,X instruction") {
      auto instruction = CPU::AddWithCarry<AddressingMode::ZeroPageX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ADC $42,X'") { REQUIRE(disassembly == "ADC $42,X"); }
    }

    WHEN("We disassemble an ADC absolute instruction") {
      auto instruction = CPU::AddWithCarry<AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ADC $1234'") { REQUIRE(disassembly == "ADC $1234"); }
    }

    WHEN("We disassemble an ADC absolute,X instruction") {
      auto instruction = CPU::AddWithCarry<AddressingMode::AbsoluteX>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ADC $1234,X'") { REQUIRE(disassembly == "ADC $1234,X"); }
    }

    WHEN("We disassemble an ADC absolute,Y instruction") {
      auto instruction = CPU::AddWithCarry<AddressingMode::AbsoluteY>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ADC $1234,Y'") { REQUIRE(disassembly == "ADC $1234,Y"); }
    }

    WHEN("We disassemble an ADC (indirect,X) instruction") {
      auto instruction = CPU::AddWithCarry<AddressingMode::IndirectX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ADC ($42,X)'") { REQUIRE(disassembly == "ADC ($42,X)"); }
    }

    WHEN("We disassemble an ADC (indirect),Y instruction") {
      auto instruction = CPU::AddWithCarry<AddressingMode::IndirectY>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ADC ($42),Y'") { REQUIRE(disassembly == "ADC ($42),Y"); }
    }

    // SBC instructions
    WHEN("We disassemble a SBC immediate instruction") {
      auto instruction = CPU::SubtractWithCarry<AddressingMode::Immediate>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'SBC #$42'") { REQUIRE(disassembly == "SBC #$42"); }
    }

    WHEN("We disassemble a SBC zero-page instruction") {
      auto instruction = CPU::SubtractWithCarry<AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'SBC $42'") { REQUIRE(disassembly == "SBC $42"); }
    }

    WHEN("We disassemble a SBC zero-page,X instruction") {
      auto instruction = CPU::SubtractWithCarry<AddressingMode::ZeroPageX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'SBC $42,X'") { REQUIRE(disassembly == "SBC $42,X"); }
    }

    WHEN("We disassemble a SBC absolute instruction") {
      auto instruction = CPU::SubtractWithCarry<AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'SBC $1234'") { REQUIRE(disassembly == "SBC $1234"); }
    }

    WHEN("We disassemble a SBC absolute,X instruction") {
      auto instruction = CPU::SubtractWithCarry<AddressingMode::AbsoluteX>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'SBC $1234,X'") { REQUIRE(disassembly == "SBC $1234,X"); }
    }

    WHEN("We disassemble a SBC absolute,Y instruction") {
      auto instruction = CPU::SubtractWithCarry<AddressingMode::AbsoluteY>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'SBC $1234,Y'") { REQUIRE(disassembly == "SBC $1234,Y"); }
    }

    WHEN("We disassemble a SBC (indirect,X) instruction") {
      auto instruction = CPU::SubtractWithCarry<AddressingMode::IndirectX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'SBC ($42,X)'") { REQUIRE(disassembly == "SBC ($42,X)"); }
    }

    WHEN("We disassemble a SBC (indirect),Y instruction") {
      auto instruction = CPU::SubtractWithCarry<AddressingMode::IndirectY>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'SBC ($42),Y'") { REQUIRE(disassembly == "SBC ($42),Y"); }
    }

    // AND instructions
    WHEN("We disassemble an AND immediate instruction") {
      auto instruction = CPU::LogicalAND<AddressingMode::Immediate>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'AND #$42'") { REQUIRE(disassembly == "AND #$42"); }
    }

    WHEN("We disassemble an AND zero-page instruction") {
      auto instruction = CPU::LogicalAND<AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'AND $42'") { REQUIRE(disassembly == "AND $42"); }
    }

    WHEN("We disassemble an AND zero-page,X instruction") {
      auto instruction = CPU::LogicalAND<AddressingMode::ZeroPageX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'AND $42,X'") { REQUIRE(disassembly == "AND $42,X"); }
    }

    WHEN("We disassemble an AND absolute instruction") {
      auto instruction = CPU::LogicalAND<AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'AND $1234'") { REQUIRE(disassembly == "AND $1234"); }
    }

    WHEN("We disassemble an AND absolute,X instruction") {
      auto instruction = CPU::LogicalAND<AddressingMode::AbsoluteX>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'AND $1234,X'") { REQUIRE(disassembly == "AND $1234,X"); }
    }

    WHEN("We disassemble an AND absolute,Y instruction") {
      auto instruction = CPU::LogicalAND<AddressingMode::AbsoluteY>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'AND $1234,Y'") { REQUIRE(disassembly == "AND $1234,Y"); }
    }

    WHEN("We disassemble an AND (indirect,X) instruction") {
      auto instruction = CPU::LogicalAND<AddressingMode::IndirectX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'AND ($42,X)'") { REQUIRE(disassembly == "AND ($42,X)"); }
    }

    WHEN("We disassemble an AND (indirect),Y instruction") {
      auto instruction = CPU::LogicalAND<AddressingMode::IndirectY>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'AND ($42),Y'") { REQUIRE(disassembly == "AND ($42),Y"); }
    }

    // EOR instructions
    WHEN("We disassemble an EOR immediate instruction") {
      auto instruction = CPU::ExclusiveOR<AddressingMode::Immediate>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'EOR #$42'") { REQUIRE(disassembly == "EOR #$42"); }
    }

    WHEN("We disassemble an EOR zero-page instruction") {
      auto instruction = CPU::ExclusiveOR<AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'EOR $42'") { REQUIRE(disassembly == "EOR $42"); }
    }

    WHEN("We disassemble an EOR zero-page,X instruction") {
      auto instruction = CPU::ExclusiveOR<AddressingMode::ZeroPageX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'EOR $42,X'") { REQUIRE(disassembly == "EOR $42,X"); }
    }

    WHEN("We disassemble an EOR absolute instruction") {
      auto instruction = CPU::ExclusiveOR<AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'EOR $1234'") { REQUIRE(disassembly == "EOR $1234"); }
    }

    WHEN("We disassemble an EOR absolute,X instruction") {
      auto instruction = CPU::ExclusiveOR<AddressingMode::AbsoluteX>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'EOR $1234,X'") { REQUIRE(disassembly == "EOR $1234,X"); }
    }

    WHEN("We disassemble an EOR absolute,Y instruction") {
      auto instruction = CPU::ExclusiveOR<AddressingMode::AbsoluteY>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'EOR $1234,Y'") { REQUIRE(disassembly == "EOR $1234,Y"); }
    }

    WHEN("We disassemble an EOR (indirect,X) instruction") {
      auto instruction = CPU::ExclusiveOR<AddressingMode::IndirectX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'EOR ($42,X)'") { REQUIRE(disassembly == "EOR ($42,X)"); }
    }

    WHEN("We disassemble an EOR (indirect),Y instruction") {
      auto instruction = CPU::ExclusiveOR<AddressingMode::IndirectY>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'EOR ($42),Y'") { REQUIRE(disassembly == "EOR ($42),Y"); }
    }

    // ORA instructions
    WHEN("We disassemble an ORA immediate instruction") {
      auto instruction = CPU::BitwiseOR<AddressingMode::Immediate>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ORA #$42'") { REQUIRE(disassembly == "ORA #$42"); }
    }

    WHEN("We disassemble an ORA zero-page instruction") {
      auto instruction = CPU::BitwiseOR<AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ORA $42'") { REQUIRE(disassembly == "ORA $42"); }
    }

    WHEN("We disassemble an ORA zero-page,X instruction") {
      auto instruction = CPU::BitwiseOR<AddressingMode::ZeroPageX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ORA $42,X'") { REQUIRE(disassembly == "ORA $42,X"); }
    }

    WHEN("We disassemble an ORA absolute instruction") {
      auto instruction = CPU::BitwiseOR<AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ORA $1234'") { REQUIRE(disassembly == "ORA $1234"); }
    }

    WHEN("We disassemble an ORA absolute,X instruction") {
      auto instruction = CPU::BitwiseOR<AddressingMode::AbsoluteX>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ORA $1234,X'") { REQUIRE(disassembly == "ORA $1234,X"); }
    }

    WHEN("We disassemble an ORA absolute,Y instruction") {
      auto instruction = CPU::BitwiseOR<AddressingMode::AbsoluteY>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ORA $1234,Y'") { REQUIRE(disassembly == "ORA $1234,Y"); }
    }

    WHEN("We disassemble an ORA (indirect,X) instruction") {
      auto instruction = CPU::BitwiseOR<AddressingMode::IndirectX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ORA ($42,X)'") { REQUIRE(disassembly == "ORA ($42,X)"); }
    }

    WHEN("We disassemble an ORA (indirect),Y instruction") {
      auto instruction = CPU::BitwiseOR<AddressingMode::IndirectY>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ORA ($42),Y'") { REQUIRE(disassembly == "ORA ($42),Y"); }
    }

    // ASL instructions
    WHEN("We disassemble an ASL accumulator instruction") {
      auto instruction = CPU::ShiftLeft<AddressingMode::Accumulator>{0};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ASL A'") { REQUIRE(disassembly == "ASL A"); }
    }

    WHEN("We disassemble an ASL zero-page instruction") {
      auto instruction = CPU::ShiftLeft<AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ASL $42'") { REQUIRE(disassembly == "ASL $42"); }
    }

    WHEN("We disassemble an ASL zero-page,X instruction") {
      auto instruction = CPU::ShiftLeft<AddressingMode::ZeroPageX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ASL $42,X'") { REQUIRE(disassembly == "ASL $42,X"); }
    }

    WHEN("We disassemble an ASL absolute instruction") {
      auto instruction = CPU::ShiftLeft<AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ASL $1234'") { REQUIRE(disassembly == "ASL $1234"); }
    }

    WHEN("We disassemble an ASL absolute,X instruction") {
      auto instruction = CPU::ShiftLeft<AddressingMode::AbsoluteX>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ASL $1234,X'") { REQUIRE(disassembly == "ASL $1234,X"); }
    }

    // LSR instructions
    WHEN("We disassemble an LSR accumulator instruction") {
      auto instruction = CPU::ShiftRight<AddressingMode::Accumulator>{0};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LSR A'") { REQUIRE(disassembly == "LSR A"); }
    }

    WHEN("We disassemble an LSR zero-page instruction") {
      auto instruction = CPU::ShiftRight<AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LSR $42'") { REQUIRE(disassembly == "LSR $42"); }
    }

    WHEN("We disassemble an LSR zero-page,X instruction") {
      auto instruction = CPU::ShiftRight<AddressingMode::ZeroPageX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LSR $42,X'") { REQUIRE(disassembly == "LSR $42,X"); }
    }

    WHEN("We disassemble an LSR absolute instruction") {
      auto instruction = CPU::ShiftRight<AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LSR $1234'") { REQUIRE(disassembly == "LSR $1234"); }
    }

    WHEN("We disassemble an LSR absolute,X instruction") {
      auto instruction = CPU::ShiftRight<AddressingMode::AbsoluteX>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LSR $1234,X'") { REQUIRE(disassembly == "LSR $1234,X"); }
    }

    // ROR instructions
    WHEN("We disassemble a ROR accumulator instruction") {
      auto instruction = CPU::RotateRight<AddressingMode::Accumulator>{0};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ROR A'") { REQUIRE(disassembly == "ROR A"); }
    }

    WHEN("We disassemble a ROR zero-page instruction") {
      auto instruction = CPU::RotateRight<AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ROR $42'") { REQUIRE(disassembly == "ROR $42"); }
    }

    WHEN("We disassemble a ROR zero-page,X instruction") {
      auto instruction = CPU::RotateRight<AddressingMode::ZeroPageX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ROR $42,X'") { REQUIRE(disassembly == "ROR $42,X"); }
    }

    WHEN("We disassemble a ROR absolute instruction") {
      auto instruction = CPU::RotateRight<AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ROR $1234'") { REQUIRE(disassembly == "ROR $1234"); }
    }

    WHEN("We disassemble a ROR absolute,X instruction") {
      auto instruction = CPU::RotateRight<AddressingMode::AbsoluteX>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ROR $1234,X'") { REQUIRE(disassembly == "ROR $1234,X"); }
    }

    // ROL instructions
    WHEN("We disassemble a ROL accumulator instruction") {
      auto instruction = CPU::RotateLeft<AddressingMode::Accumulator>{0};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ROL A'") { REQUIRE(disassembly == "ROL A"); }
    }

    WHEN("We disassemble a ROL zero-page instruction") {
      auto instruction = CPU::RotateLeft<AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ROL $42'") { REQUIRE(disassembly == "ROL $42"); }
    }

    WHEN("We disassemble a ROL zero-page,X instruction") {
      auto instruction = CPU::RotateLeft<AddressingMode::ZeroPageX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ROL $42,X'") { REQUIRE(disassembly == "ROL $42,X"); }
    }

    WHEN("We disassemble a ROL absolute instruction") {
      auto instruction = CPU::RotateLeft<AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ROL $1234'") { REQUIRE(disassembly == "ROL $1234"); }
    }

    WHEN("We disassemble a ROL absolute,X instruction") {
      auto instruction = CPU::RotateLeft<AddressingMode::AbsoluteX>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'ROL $1234,X'") { REQUIRE(disassembly == "ROL $1234,X"); }
    }

    // INC instructions
    WHEN("We disassemble an INC zero-page instruction") {
      auto instruction = CPU::Increment<AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'INC $42'") { REQUIRE(disassembly == "INC $42"); }
    }

    WHEN("We disassemble an INC zero-page,X instruction") {
      auto instruction = CPU::Increment<AddressingMode::ZeroPageX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'INC $42,X'") { REQUIRE(disassembly == "INC $42,X"); }
    }

    WHEN("We disassemble an INC absolute instruction") {
      auto instruction = CPU::Increment<AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'INC $1234'") { REQUIRE(disassembly == "INC $1234"); }
    }

    WHEN("We disassemble an INC absolute,X instruction") {
      auto instruction = CPU::Increment<AddressingMode::AbsoluteX>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'INC $1234,X'") { REQUIRE(disassembly == "INC $1234,X"); }
    }

    // DEC instructions
    WHEN("We disassemble a DEC zero-page instruction") {
      auto instruction = CPU::Decrement<AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'DEC $42'") { REQUIRE(disassembly == "DEC $42"); }
    }

    WHEN("We disassemble a DEC zero-page,X instruction") {
      auto instruction = CPU::Decrement<AddressingMode::ZeroPageX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'DEC $42,X'") { REQUIRE(disassembly == "DEC $42,X"); }
    }

    WHEN("We disassemble a DEC absolute instruction") {
      auto instruction = CPU::Decrement<AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'DEC $1234'") { REQUIRE(disassembly == "DEC $1234"); }
    }

    WHEN("We disassemble a DEC absolute,X instruction") {
      auto instruction = CPU::Decrement<AddressingMode::AbsoluteX>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'DEC $1234,X'") { REQUIRE(disassembly == "DEC $1234,X"); }
    }

    WHEN("We disassemble a SBC immediate instruction") {
      auto instruction = CPU::SubtractWithCarry<AddressingMode::Immediate>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'SBC #$42'") { REQUIRE(disassembly == "SBC #$42"); }
    }

    WHEN("We disassemble an undocumented SBC immediate instruction") {
      auto instruction = CPU::SubtractWithCarry<AddressingMode::Immediate>{0x42, true};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to '*SBC #$42'") { REQUIRE(disassembly == "*SBC #$42"); }
    }
  }
}
